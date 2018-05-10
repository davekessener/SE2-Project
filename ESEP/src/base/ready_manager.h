#ifndef SRC_BASE_READY_MANAGER_H
#define SRC_BASE_READY_MANAGER_H

#include "base/IManager.h"
#include "communication/packet.h"
#include "communication/IRecipient.h"
#include "hal/hal.h"
#include "lib/timer.h"
#include "hal/buttons.h"

#include "system.h"

namespace esep
{
 	 namespace base
	 {
 	   class ReadyManager : public IManager
	   {
	   	  public:
 		   ReadyManager(communication::IRecipient *);
 		   void accept(std::shared_ptr<communication::Packet>);
 		   void handle(hal::HAL::Event);

	   	  private:
 		  communication::IRecipient * const mBaseHandler;
 		  hal::Buttons& BUTTONS;


	   };
	 }
}



#endif /* SRC_BASE_READY_MANAGER_H */
