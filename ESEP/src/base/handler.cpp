#include "handler.h"

namespace esep { namespace base {

Handler::Handler(communication::IRecipient* communicationModule)
	: mCommunicationModul(communicationModule)
{
	mConfigManager.reset(new ConfigManager(this, &mConfigData));

	mCurrentManager = mConfigManager.get();
//	mCurrentManager = mDefaultManager.get();

	//todo thread
}

void Handler::accept(std::shared_ptr<communication::Packet> packet)
{
	if(packet->target() == communication::Packet::Location::MASTER)
	{
		mCommunicationModul->accept(packet);
	}
	else
	{
		switch(packet->message())
		{
			case(communication::Packet::Message::SELECT_CONFIG):
				switchManager(mConfigManager.get());
				break;

			case(communication::Packet::Message::SELECT_RUN):
				switchManager(mRunManager.get());
				break;

			// TODO Error and DefaultManager

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
	mCurrentManager->handle(e);
}

}}
