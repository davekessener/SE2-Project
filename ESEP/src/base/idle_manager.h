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
 		   void accept(std::shared_ptr<communication::Packet>);
 		   void handle(hal::HAL::Event);

 			enum class State : int8_t
 			{
 				IS_PRESSED,
				IS_NOT_PRESSED,
				READY_RUN,
				READY_CONFIG,
				RUN,
				CONFIG
 			};

	   	  private:
 		  communication::IRecipient * const mBaseHandler;
 		  State mState;
 		  hal::Buttons& BUTTONS;


	   };
	 }
}

#endif /* SRC_BASE_IDLE_MANAGER_H */
