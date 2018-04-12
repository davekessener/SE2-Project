#ifndef ESEP_HAL_SWITCH_H
#define ESEP_HAL_SWITCH_H

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class Switch
		{
			public:
				Switch(HAL *);
				~Switch( );
				void open( );
				void close( );
				bool isOpen( ) const;
			private:
				HAL *mHAL;
		};
	}
}

#endif
