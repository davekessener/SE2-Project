#ifndef ESEP_MASTER_HSM_BASE_H
#define ESEP_MASTER_HSM_BASE_H

#include "lib/utils.h"

#include "master/types.h"

namespace esep
{
	namespace master
	{
		namespace hsm
		{
			class Base
			{
				public:
				MXT_DEFINE_E(UnhandledEventException);

				public:
					virtual ~Base( ) { }
					virtual void handle(event_t);
			};
		}
	}
}

#endif
