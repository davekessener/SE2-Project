#include "handler.h"

namespace esep { namespace base {

Handler::Handler(communication::IRecipient* communicationModule)
	: mCommunicationModul(communicationModule)
, mConfigData()
, mConfigManager(nullptr)
, mRunManager(nullptr)
, mErrorHandler(nullptr)
, mCurrentManager(nullptr)
{
	mConfigManager = new ConfigManager(this, mConfigData);
	mRunManager = new RunManagerDummy(this, mConfigData);
	mErrorHandler = new ErrorManagerDummy(this);
	mCurrentManager = mRunManager;

	//todo thread
}

Handler::~Handler()
{

}

void Handler::accept(std::shared_ptr<communication::Packet> packet)
{
	if(packet->target() == communication::Packet::Location::MASTER)
	{
		mCommunicationModul->accept(packet);
	} else {
		switch(packet->message())
		{
			case(communication::Packet::Message::START_CONFIG):
					mCurrentManager->leave();
					mCurrentManager = mConfigManager;
					mCurrentManager->enter();
			break;
			case(communication::Packet::Message::START_RUN):
					mCurrentManager->leave();
					mCurrentManager = mRunManager;
					mCurrentManager->enter();
			break;
			default:
				mCurrentManager->accept(packet);
		}
	}
}

}}
