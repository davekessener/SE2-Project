
#ifndef SRC_BASE_IMANAGER_H
#define SRC_BASE_IMANAGER_H

#include "communication/IRecipient.h"
#include "hal/hal.h"

namespace esep
{
	namespace base
	{
		class IManager : public communication::IRecipient
		{
			public:
			   IManager();
			   virtual ~IManager()
			   {};
			   virtual void enter()
			   {};
			   virtual void leave()
			   {};
			   virtual void handle(hal::HAL::Event)
			   {};
		};

}
}

#endif
