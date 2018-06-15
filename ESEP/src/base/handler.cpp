#include "base/handler.h"

#include "qnx/channel.h"
#include "qnx/connection.h"
#include "qnx/pulse.h"

#include "lib/logger.h"

#include "base/idle/idle_manager.h"
#include "base/idle/ready_manager.h"
#include "base/idle/valid_manager.h"

#include "base/config_manager.h"
#include "base/error_manager.h"
#include "base/run_manager.h"

#include "data/data_point.h"
#include "data/message_data.h"

#include "hal.h"

namespace esep { namespace base {

namespace
{
	typedef data::Data_ptr Data_ptr;
}

typedef hal::Buttons::Button Button;

Handler::Handler(communication::IRecipient *m, ConfigObject *co)
	: mMaster(m)
	, mConfigData(co)
	, mRunning(true)
{
	mIdleManager.reset(new IdleManager(this, mConfigData));
	mValidManager.reset(new ValidManager(this));
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

			mCurrentManager->enter();

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
						processBase(packet);
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
					processHAL(static_cast<hal::HAL::Event>(pulse.value));
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
				MXT_LOG_WARN(e.what());
			}

//			mCurrentManager->leave();

			mConnection.close();
		}
		MXT_CATCH_ALL_STRAY
	});
}

Handler::~Handler()
{
	if(mRunning.load()) try
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

//	MXT_LOG("Received packet ", p);

	if(p->target() == Location::MASTER)
	{
		mMaster->accept(p);
	}
	else
	{
		if(mRunning.load())
		{
			mPacketBuffer.insert(p);
			mConnection.sendPulse(MessageType::PACKET_IN_BUFFER);
		}
	}
}

void Handler::handle(Event e)
{
	if(e != hal::HAL::Event::HEIGHT_SENSOR)
	{
		MXT_LOG_INFO("Received HAL event ", lib::hex<32>(e));
	}

	if(mRunning.load())
	{
		mConnection.sendPulse(static_cast<int8_t>(MessageType::HAL_EVENT), static_cast<uint32_t>(e));
	}
}

// # ---------------------------------------------------------------------------------------------------

void Handler::doSwitch(IManager *m)
{
	if (m != mCurrentManager)
	{
		mCurrentManager->leave();
		mCurrentManager = m;
		mCurrentManager->enter();
	}
}

void Handler::processBase(Packet_ptr p)
{
	switch(p->message().as<Message::Base>())
	{
	case Message::Base::SHUTDOWN:
		MXT_LOG("Shutting down ...");
		mRunning = false;
		break;

	case Message::Base::RUN:
		MXT_LOG_INFO("Switching to Run ...");
		doSwitch(mRunManager.get());
		break;

	case Message::Base::CONFIG:
		MXT_LOG_INFO("Switching to Config ...");
		doSwitch(mConfigManager.get());
		break;

	case Message::Base::IDLE:
		MXT_LOG_INFO("Switching to Idle ...");
		doSwitch(mIdleManager.get());
		break;

	case Message::Base::READY:
		MXT_LOG_INFO("Switching to Ready ...");
		doSwitch(mReadyManager.get());
		break;

	case Message::Base::VALID:
		MXT_LOG_INFO("Switching to Valid ...");
		doSwitch(mValidManager.get());
		break;

	case Message::Base::PRINT:
		MXT_LOG_INFO("Printing message from master!");
		for(const auto& d : *p)
		{
			if(d->type() == data::DataPoint::Type::MESSAGE)
			{
				HAL_CONSOLE.println(static_cast<data::Message&>(*d).message());
			}
		}
		break;
	}
}

void Handler::processHAL(Event e)
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
