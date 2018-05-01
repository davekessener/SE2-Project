#ifndef SRC_BASE_HANDLER_H
#define SRC_BASE_HANDLER_H

#include <memory>

#include "base/IManager.h"
#include "communication/IRecipient.h"

namespace esep
{
	namespace base
	{
		class Handler : public communication::IRecipient
		{
			public:
				Handler(std::shared_ptr<communication::IRecipient> CommunicationModul); //requires the IRecipient of the higher comm. layer
				~Handler();
				void accept(std::shared_ptr<Packet>);
				void operator()(hal::HAL::Event); //for event listening registration

			private:
				std::shared_ptr<communication::IRecipient> mCommunicationModul;
				communication::IRecipient mRunManager;
				communication::IRecipient mConfigManager;
				communication::IRecipient mErrorHandler;
				IManager mCurrentManager;
				ConfigObject mConfigData;

		};
	}
}

#endif /* SRC_BASE_HANDLER_H */
