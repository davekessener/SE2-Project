#ifndef SRC_BASE_HANDLER_H
#define SRC_BASE_HANDLER_H

#include <memory>

#include "base/IManager.h"
#include "communication/IRecipient.h"
#include "run_manager_dummy.h"
#include "config_manager.h"
#include "error_manager_dummy.h"
#include "config_object.h"


namespace esep
{
	namespace base
	{
		class Handler : public communication::IRecipient
		{
			public:
				Handler(communication::IRecipient* CommunicationModul); //requires the IRecipient of the higher comm. layer
				~Handler();
				void operator()(hal::HAL::Event); //for event listening registration
				void accept(std::shared_ptr<communication::Packet>);


			private:
				communication::IRecipient* mCommunicationModul;
				ConfigObject mConfigData;
				ConfigManager* mConfigManager;
				RunManagerDummy* mRunManager;
				ErrorManagerDummy* mErrorHandler;
				IManager* mCurrentManager;


		};
	}
}

#endif /* SRC_BASE_HANDLER_H */
