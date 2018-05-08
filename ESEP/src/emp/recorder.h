#ifndef ESEP_EMP_RECORDER_H
#define ESEP_EMP_RECORDER_H

#include <memory>

#include "hal/HAL.h"
#include "hal/physical.h"

#include "lib/writer.h"

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
			typedef std::unique_ptr<lib::Writer> Writer_ptr;

			public:
				Recorder(Writer_ptr);
				void setCallback(callback_t f) override { mCallback = f; }

			private:
				void record(uint64_t, Event);

			private:
				Writer_ptr mWriter;
				callback_t mCallback;
		};
	}
}

#endif
