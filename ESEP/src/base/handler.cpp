#include "handler.h"
#include "qnx/channel.h"
#include "qnx/connection.h"
#include "qnx/pulse.h"
#include "lib/logger.h"

namespace esep { namespace base {

Handler::Handler(communication::IRecipient* communicationModule)
	: mCommunicationModul(communicationModule)
	, mRunning(true)
{
	mConfigManager.reset(new ConfigManager(this, &mConfigData));

	mCurrentManager = mConfigManager.get();
//	mCurrentManager = mDefaultManager.get();

	//Start the Base Handler Thread
	mHandlerThread.construct([this](void){
		qnx::Channel channel;
		mConnection = channel.connect();

		while(mRunning)
		{
			qnx::pulse_t pulse = channel.receivePulse(); //wait for pulse message
			//Look for the Code
			HandlerThreadCtrl threadCtrl = static_cast<HandlerThreadCtrl>(pulse.code);

			//It can be a command to switch the Manager
			if(threadCtrl == HandlerThreadCtrl::SWITCH_MANAGER)
			{
				Message msg = static_cast<Message>(pulse.value);
				switch(msg)
				{
					case(Message::START_RUN):
							switchManager(mRunManager.get());
						break;

					case(Message::START_CONFIG):
							switchManager(mConfigManager.get());
						break;

					case(Message::IDLE):
							switchManager(mDefaultManager.get());
						break;

					case(Message::SERIAL_ERROR):
							switchManager(mErrorManager.get());
						break;

					//TODO: Handle new ErrorStates

					default:
						MXT_LOG(lib::stringify("Received pulse {", lib::hex<8>(pulse.code), ", ", lib::hex<32>(pulse.value), "}"));
					break;
				}
			}
			// It can be a command to forward a Hal Event
			else if (threadCtrl == HandlerThreadCtrl::HAL_EVENT)
			{
				hal::HAL::Event event = static_cast<hal::HAL::Event>(pulse.value);
				mCurrentManager->handle(event);
			}
			// or it can be a command to destroy the thread
			else
			{
				mRunning = false;
			}
		}

		mConnection.close();
	});
}

Handler::~Handler()
{
	try
	{
		mConnection.sendPulse(static_cast<int8_t>(HandlerThreadCtrl::STOP_RUNNING));
	}
	catch(...)
	{
		MXT_LOG("Couldn't send shutdown signal; may hang!");
	}
}

void Handler::accept(std::shared_ptr<communication::Packet> packet)
{
	if(packet->target() == communication::Packet::Location::MASTER)
	{
		mCommunicationModul->accept(packet);
	}
	else
	{
		Message msg = packet->message();
		switch(msg)
		{
			case(Message::START_RUN): case(Message::START_CONFIG): case(Message::IDLE): case(Message::SERIAL_ERROR):
				mConnection.sendPulse(static_cast<int8_t>(HandlerThreadCtrl::SWITCH_MANAGER), static_cast<int8_t>(msg));
				break;

			default:
				mCurrentManager->accept(packet);
				break;
		}
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
	mConnection.sendPulse(static_cast<int8_t>(HandlerThreadCtrl::HAL_EVENT), static_cast<uint32_t>(e));
}

}}
