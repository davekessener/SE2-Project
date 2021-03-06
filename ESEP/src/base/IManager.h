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
			typedef hal::HAL::Event Event;

			public:
				virtual ~IManager( ) { };

				virtual void enter( ) { };
				virtual void leave( ) { };
				virtual void handle(Event) { };
		};
	}
}

#endif
