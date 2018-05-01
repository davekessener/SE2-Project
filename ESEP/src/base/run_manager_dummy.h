#ifndef SRC_BASE_RUN_MANAGER_H
#define SRC_BASE_RUN_MANAGER_H



#include "communication/IRecipient.h"
#include "base/config_object.h"
#include "IManager.h"

namespace esep
{
	namespace base
	{
		class RunManagerDummy : public IManager
		{
			public:
				RunManagerDummy(communication::IRecipient*, ConfigObject){};
				~RunManagerDummy(){};
				void accept(std::shared_ptr<communication::Packet>){};
				void enter(){};
				void leave(){};
				void handle(hal::HAL::Event){};
		};
	}
}



#endif /* SRC_BASE_RUN_MANAGER_H */
