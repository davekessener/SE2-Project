#ifndef SRC_BASE_ERROR_MANAGER_DUMMY_H
#define SRC_BASE_ERROR_MANAGER_DUMMY_H



#include "communication/IRecipient.h"
#include "base/config_object.h"
#include "IManager.h"

namespace esep
{
	namespace base
	{
		class ErrorManagerDummy : public IManager
		{
			public:
				ErrorManagerDummy(communication::IRecipient*){};
				~ErrorManagerDummy(){};
				void accept(std::shared_ptr<communication::Packet>){};
				void enter(){};
				void leave(){};
				void handle(hal::HAL::Event){};
		};
	}
}



#endif /* SRC_BASE_ERROR_MANAGER_DUMMY_H */
