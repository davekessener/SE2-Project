#include "base/handler.h"

#include "qnx/channel.h"
#include "qnx/connection.h"
#include "qnx/pulse.h"

#include "lib/logger.h"

#include "base/idle/idle_manager.h"
#include "base/idle/ready_manager.h"

#include "base/config_manager.h"
#include "base/error_manager.h"
#include "base/run_manager.h"

#include "hal.h"

namespace esep { namespace base {

typedef hal::Buttons::Button Button;

Handler::Handler(ConfigObject *co)
	: mMaster(nullptr)
	, mConfigData(co)
	, mRunning(true)
{
	mIdleManager.reset(new IdleManager(this, mConfigData));
	mReadyManager.reset(new ReadyManager(this));
	mConfigManager.reset(new ConfigManager(this, mConfigData));
	mRunManager.reset(new RunManager(this, mConfigData));
	mErrorManager.reset(new ErrorManager(this));

	mCurrentManager = mIdleManager.get();

	//Start the Base Handler Thread
	mHandlerThread.construct([this](void) {
		try
		{
			qnx::Channel channel;

			mConnection = channel.connect();

			while(mRunning.load()) try
			{
				qnx::pulse_t pulse = channel.receivePulse(); //wait for pulse message

				switch(pulse.code)
				{
				case(static_cast<int8_t>(MessageType::PACKET_IN_BUFFER)):
				{
					auto packet = mPacketBuffer.remove();
					auto msg = packet->message();

					if(msg.is<Message::Base>())
					{
						handleBase(msg.as<Message::Base>());
					}
					else if(msg.is<Message::Error>())
					{
						doSwitch(mErrorManager.get());
						mCurrentManager->accept(packet);
					}
					else
					{
						mCurrentManager->accept(packet);
					}
				}
				break;

				case(static_cast<int8_t>(MessageType::HAL_EVENT)):
					handleHAL(static_cast<hal::HAL::Event>(pulse.value));
					break;

				case(static_cast<int8_t>(MessageType::STOP_RUNNING)):
					mRunning = false;
					break;

				default:
					MXT_LOG_WARN("Received unexpected pulse {", lib::hex<8>(pulse.code), ", ", lib::hex<32>(pulse.value), "}");
					break;
				}
			}
			catch(const std::exception& e)
			{
				MXT_LOG_WARN("Caught an exception from manager: ", e.what());
			}

			mCurrentManager->leave();

			mConnection.close();
		}
		MXT_CATCH_ALL_STRAY
	});
}

Handler::~Handler()
{
	try
	{
		mConnection.sendPulse(static_cast<int8_t>(MessageType::STOP_RUNNING));
	}
	catch(...)
	{
		MXT_LOG("Couldn't send shutdown signal; may hang!");
	}
}


void Handler::accept(Packet_ptr p)
{
	if(!mMaster)
	{
		MXT_THROW_EX(UndefinedMasterException);
	}

	MXT_LOG("Received packet {", (int)p->source(), " -> ", (int)p->target(), ", msg: ", lib::hex<16>(p->message()), "}");

	if(p->target() == Location::MASTER)
	{
		mMaster->accept(p);
	}
	else
	{
		mPacketBuffer.insert(p);
		mConnection.sendPulse(MessageType::PACKET_IN_BUFFER);
	}
}

void Handler::handleBase(Message::Base msg)
{
	switch(msg)
	{
	case Message::Base::SHUTDOWN:
		MXT_LOG("Shutting down ...");
		mRunning = false;
		break;

	case Message::Base::RUN:
		doSwitch(mRunManager.get());
		break;

	case Message::Base::CONFIG:
		doSwitch(mConfigManager.get());
		break;

	case Message::Base::IDLE:
		doSwitch(mIdleManager.get());
		break;

	case Message::Base::READY:
		doSwitch(mReadyManager.get());
		break;
	}
}

void Handler::doSwitch(IManager *m)
{
	if (m != mCurrentManager)
	{
		if (m == mRunManager.get()) { MXT_LOG_INFO("Switching to Run"); }
		else if (m == mConfigManager.get()) { MXT_LOG_INFO("Switching to Config"); }
		else if (m == mIdleManager.get()) { MXT_LOG_INFO("Switching to Idle"); }
		else if (m == mReadyManager.get()) { MXT_LOG_INFO("Switching to Ready"); }
		else if (m == mErrorManager.get()) { MXT_LOG_INFO("Switching to Error"); }

		mCurrentManager->leave();
		mCurrentManager = m;
		mCurrentManager->enter();
	}
}

void Handler::handle(Event e)
{
	MXT_LOG_INFO("Received HAL event ", lib::hex<32>(e));

	mConnection.sendPulse(static_cast<int8_t>(MessageType::HAL_EVENT), static_cast<uint32_t>(e));
}

void Handler::handleHAL(Event e)
{
	if(e == Event::BTN_ESTOP && HAL_BUTTONS.isPressed(Button::ESTOP))
	{
		mMaster->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Error::ESTOP));
	}
	else
	{
		mCurrentManager->handle(e);
	}
}

}}
