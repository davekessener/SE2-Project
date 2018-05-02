#ifndef SRC_BASE_HANDLER_H
#define SRC_BASE_HANDLER_H

#include <memory>

#include "base/IManager.h"
#include "communication/IRecipient.h"
#include "config_manager.h"
#include "config_object.h"


namespace esep
{
	namespace base
	{
		class Handler : public communication::IRecipient
		{
			public:
				Handler(communication::IRecipient* CommunicationModul); //requires the IRecipient of the higher comm. layer
				void handle(hal::HAL::Event); //for event listening registration
				void accept(std::shared_ptr<communication::Packet>);

			private:
				void switchManager(IManager *);

			private:
				communication::IRecipient* mCommunicationModul;
				ConfigObject mConfigData;
				std::shared_ptr<IManager> mConfigManager,  mRunManager, mErrorManager, mDefaultManager;
				IManager* mCurrentManager;
		};
	}
}

#endif /* SRC_BASE_HANDLER_H */
