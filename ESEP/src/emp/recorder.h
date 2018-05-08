#ifndef ESEP_EMP_RECORDER_H
#define ESEP_EMP_RECORDER_H

#include "hal/HAL.h"
#include "hal/physical.h"

namespace esep
{
	namespace emp
	{
		class Recorder : public hal::Physical
		{
			typedef hal::HAL::Field Field;
			typedef hal::HAL::callback_t callback_t;
			typedef hal::HAL::bitmask_t bitmask_t;
			typedef hal::HAL::Event Event;

			public:
				Recorder( );
				void setCallback(callback_t f) override { mCallback = f; }

			private:
				void record(uint64_t, Event);

			private:
				hal::HAL * const mHAL;
				callback_t mCallback;
		};
	}
}

#endif
