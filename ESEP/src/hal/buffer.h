#ifndef ESEP_HAL_BUFFER_H
#define ESEP_HAL_BUFFER_H

#include <map>

#include "lib/utils.h"

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class Buffer : public HAL
		{
			public:
			using HAL::Field;

			public:
				uint32_t in(Field f) { return mFields[static_cast<uint>(f)]; }
				virtual void update(Field, uint32_t);
				virtual void setCallback(callback_t f) { mSubscriber = f; }
			private:
				uint32_t mFields[HAL::N_FIELDS];
				callback_t mSubscriber;
		};
	}
}

#endif

