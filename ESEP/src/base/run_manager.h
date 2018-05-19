#ifndef SRC_BASE_RUN_MANAGER_H_
#define SRC_BASE_RUN_MANAGER_H_

#include "communication/packet.h"
#include "IManager.h"


namespace esep
{
	namespace base
	{
		class RunManager : public IManager
		{
			public:
			   RunManager() {};
			   void enter() override;
			   void leave() override;
			   void handle(hal::HAL::Event) override;
			   void accept(Packet_ptr) override;			   
		};
	}
}



#endif /* SRC_BASE_RUN_MANAGER_H_ */
