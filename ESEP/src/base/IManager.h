/*
 * IManager.h
 *

 */

#ifndef SRC_BASE_IMANAGER_H_
#define SRC_BASE_IMANAGER_H_

#include "communication/IRecipient.h"
#include "hal/hal.h"

namespace esep
{
	namespace base
	{
		class IManager : public communication::IRecipient
		{
			public:
			   virtual ~IManager(){}
			   virtual void enter();
			   virtual void leave();
			   virtual void handle(hal::HAL::Event);

		};

}
}



#endif /* SRC_BASE_IMANAGER_H_ */
