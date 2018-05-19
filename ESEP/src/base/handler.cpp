#include "base/handler.h"

#include "qnx/channel.h"
#include "qnx/connection.h"
#include "qnx/pulse.h"

#include "lib/logger.h"

#include "base/config_manager.h"
#include "base/idle_manager.h"
#include "base/ready_manager.h"
#include "base/error_manager.h"

#include "system.h"

namespace esep { namespace base {

typedef hal::Buttons::Button Button;

Handler::Handler( )
	: mMaster(nullptr)
	, mRunning(true)
{
	mIdleManager.reset(new IdleManager(this));
	mReadyManager.reset(new ReadyManager(this));
	mConfigManager.reset(new ConfigManager(this, &mConfigData));

	mCurrentManager = mIdleManager.get();

	//Start the Base Handler Thread
	mHandlerThread.construct([this](void) {
		try
		{
			qnx::Channel channel;

			mConnection = channel.connect();

			while(mRunning.load())
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
						switchManager(msg.as<Message::Base>());
					}
					else if(msg.is<Message::Error>())
					{
						handleError(msg.as<Message::Error>(), packet);
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

void Handler::switchManager(Message::Base msg)
{
	auto m = mCurrentManager;

	switch(msg)
	{
	case Message::Base::SHUTDOWN:
		MXT_LOG("Shutting down ...");
		mRunning = false;
		break;

	case Message::Base::RUN:
		MXT_LOG_WARN("RunManager not implemented yet!");
		mRunning = false;
//		m = mRunManager.get(); TODO
		break;

	case Message::Base::CONFIG:
		MXT_LOG_INFO("Switching to ConfigManager");
		m = mConfigManager.get();
		break;

	case Message::Base::IDLE:
		MXT_LOG_INFO("Switching to Idle");
		m = mIdleManager.get();
		break;

	case Message::Base::READY:
		MXT_LOG_INFO("Switching to Ready");
		m = mReadyManager.get();
		break;
	}

	if(mCurrentManager != m)
	{
		mCurrentManager->leave();
		mCurrentManager = m;
		mCurrentManager->enter();
	}

	if(mCurrentManager != mErrorManager.get())
	{
		mErrorManager.reset();
	}
}

void Handler::handle(Event e)
{
	MXT_LOG_INFO("Received HAL event ", lib::hex<32>(e));

	mConnection.sendPulse(static_cast<int8_t>(MessageType::HAL_EVENT), static_cast<uint32_t>(e));
}

void Handler::handleError(Message::Error e, Packet_ptr p)
{
	MXT_LOG_INFO("Received error message ", e, "!");

	auto m = ErrorManager::create(this, p);

	if(!static_cast<bool>(mErrorManager) || m->priority() >= mErrorManager->priority())
	{
		mCurrentManager->leave();
		mErrorManager = std::move(m);
		mCurrentManager = mErrorManager.get();
		mCurrentManager->enter();
	}
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
