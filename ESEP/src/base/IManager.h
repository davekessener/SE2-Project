
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
			   virtual ~IManager() { };
			   virtual void enter() = 0;
			   virtual void leave() = 0;
			   virtual void handle(hal::HAL::Event) = 0;
		};

}
}

#endif
