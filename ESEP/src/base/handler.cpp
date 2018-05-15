#include "base/handler.h"

#include "qnx/channel.h"
#include "qnx/connection.h"
#include "qnx/pulse.h"

#include "lib/logger.h"

#include "base/config_manager.h"
#include "base/idle_manager.h"
#include "base/ready_manager.h"

namespace esep { namespace base {

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

						switch(packet->message())
						{
						case(Message::SHUTDOWN):
							mRunning = false;
							break;
						case(Message::SELECT_CONFIG):
							switchManager(mConfigManager.get());
							break;

						case(Message::SELECT_RUN):
							MXT_LOG_WARN("Run manager not implemented yet!");
//							switchManager(mRunManager.get());
							break;

						case(Message::IDLE):
							switchManager(mIdleManager.get());
							break;

						case(Message::ERROR_SERIAL):
							MXT_LOG_FATAL("Serial connection failed! Shutting down ...");
							mRunning = false;
							break;

						//TODO: React to new Massage types.

						default:
							mCurrentManager->accept(packet);
							break;
						}
						break;
					}
					case(static_cast<int8_t>(MessageType::HAL_EVENT)):
						mCurrentManager->handle(static_cast<hal::HAL::Event>(pulse.value));
						break;

					case(static_cast<int8_t>(MessageType::STOP_RUNNING)):
						mRunning = false;
						break;

					default:
						// make a log, if there was an undefined MassageType
						MXT_LOG_WARN("Received unexpected pulse {", lib::hex<8>(pulse.code), ", ", lib::hex<32>(pulse.value), "}");
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


void Handler::accept(communication::Packet_ptr p)
{
	if(!mMaster)
	{
		MXT_THROW_EX(UndefinedMasterException);
	}

	MXT_LOG("Received packet {", (int)p->source(), " -> ", (int)p->target(), ", msg: ", (int)p->message());

	if(p->target() == Location::MASTER)
	{
		mMaster->accept(p);
	}
	else
	{
		mPacketBuffer.insert(p);
		mConnection.sendPulse(static_cast<int8_t>(MessageType::PACKET_IN_BUFFER));
	}
}

void Handler::switchManager(IManager *m)
{
	if(mCurrentManager != m)
	{
		mCurrentManager->leave();
		mCurrentManager = m;
		mCurrentManager->enter();
	}
}

void Handler::handle(hal::HAL::Event e)
{
	MXT_LOG("Received HAL event ", lib::hex<32>(e));

	if(e == Event::BTN_ESTOP)
	{
		accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::ESTOP));
	}
	else
	{
		mConnection.sendPulse(static_cast<int8_t>(MessageType::HAL_EVENT), static_cast<uint32_t>(e));
	}
}

}}
