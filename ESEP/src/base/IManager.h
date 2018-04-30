/*
 * IManager.h
 *

 */

#ifndef SRC_BASE_IMANAGER_H_
#define SRC_BASE_IMANAGER_H_

#include "communication/IRecipient.h"

namespace esep
{
	namespace base
	{
		class IManager : public IRecipient
		{
			public:
			   IManager();
			   virtual ~IManager()
			   {
				   delete this;
			   }
			   virtual void enter();
			   virtual void leave();
			   virtual void handle(hal::HAL::Event);

		};

}
}



#endif /* SRC_BASE_IMANAGER_H_ */
