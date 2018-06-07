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
		class Recorder : public hal::HAL
		{
			typedef hal::HAL::Field Field;
			typedef hal::HAL::callback_t callback_t;
			typedef hal::HAL::bitmask_t bitmask_t;
			typedef hal::HAL::Event Event;
			typedef std::unique_ptr<lib::Writer> Writer_ptr;

			public:
				Recorder(Writer_ptr, hal::HAL_ptr);

				uint32_t in(Field f) override { return mHAL->in(f); }
				void out(Field f, uint32_t v) override { mHAL->out(f, v); }
				void set(Field f, bitmask_t v) override { mHAL->set(f, v); }
				void reset(Field f, bitmask_t v) override { mHAL->reset(f, v); }
				void setCallback(callback_t f) override { mCallback = f; }

			private:
				void record(uint32_t, Event);

			private:
				hal::HAL_ptr mHAL;
				Writer_ptr mWriter;
				callback_t mCallback;
		};
	}
}

#endif
