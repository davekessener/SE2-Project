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

		while(mRunning.load())
		{
			qnx::pulse_t pulse = channel.receivePulse(); //wait for pulse message

			switch(pulse.code)
			{
				case(static_cast<int8_t>(MessageType::PACKET_IN_BUFFER)):
					communication::Packet_ptr packet= mPacketBuffer.remove();

					switch(packet->message())
					{
						case(Message::SELECT_CONFIG):
							switchManager(mConfigManager.get());
							break;

						case(Message::SELECT_RUN):
							switchManager(mRunManager.get());
							break;

						case(Message::IDLE):
							switchManager(mDefaultManager.get());
							break;

						//TODO: React to new Massage types.

						default:
							mCurrentManager->accept(packet);
					}
					break;

				case(static_cast<int8_t>(MessageType::HAL_EVENT)):
					hal::HAL::Event event = static_cast<hal::HAL::Event>(pulse.value);
					mCurrentManager->handle(event);
					break;

				case(static_cast<int8_t>(MessageType::STOP_RUNNING)):
					mRunning = false;
					break;

				default:
					// make a log, if there was an undefined MassageType
					MXT_LOG(lib::stringify("Received unexpected pulse {", lib::hex<8>(pulse.code), ", ", lib::hex<32>(pulse.value), "}"));
			}
		}

		mConnection.close();
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


void Handler::accept(communication::Packet_ptr packet)
{
	if(packet->target() == communication::Packet::Location::MASTER)
	{
		mCommunicationModul->accept(packet);
	}
	else
	{
		mPacketBuffer.insert(packet);
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
	mConnection.sendPulse(static_cast<int8_t>(MessageType::HAL_EVENT), static_cast<uint32_t>(e));
}

}}
