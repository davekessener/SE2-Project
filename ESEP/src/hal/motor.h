#ifndef ESEP_HAL_MOTOR_H
#define ESEP_HAL_MOTOR_H

#include "hal/hal.h"

namespace esep
{
	namespace hal
	{
		class Motor
		{
			public:
				Motor(HAL *);
				~Motor( );
				void start( );
				void stop( );
				void slow( );
				void fast( );
				void enable( );
				void disable( );
				bool isRunning( ) const { return mRunning; }
				bool isFast( ) const { return mFast; }
				bool isEnabled( ) const { return mEnabled; }
			private:
				HAL * const mHAL;
				bool mRunning, mFast, mEnabled;
		};
	}
}

#endif

