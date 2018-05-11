#ifndef SRC_BASE_IDLE_MANAGER_H
#define SRC_BASE_IDLE_MANAGER_H

#include <memory>

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
 	   class IdleManager : public IManager
	   {
	   	  public:
 		   IdleManager(communication::IRecipient *);
 		   void handle(hal::HAL::Event);


	   	  private:
 		  communication::IRecipient * const mBaseHandler;
 		  int mTime;
 		  hal::Buttons& BUTTONS;


	   };
	 }
}

#endif /* SRC_BASE_IDLE_MANAGER_H */
