#ifndef SRC_BASE_IDLE_MANAGER_H
#define SRC_BASE_IDLE_MANAGER_H

#include "base/IManager.h"
#include "communication/packet.h"
#include "hal/hal.h"

namespace esep
{
 	 namespace base
	 {
 	   class IdleManager : public IManager
	   {
	   	  public:
 		   void accept(communication::Packet);
 		   void handle(hal::HAL::Event);

	   };
	 }
}

#endif /* SRC_BASE_IDLE_MANAGER_H */
