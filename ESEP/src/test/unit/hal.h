#ifndef ESEP_TEST_UNIT_HAL_H
#define ESEP_TEST_UNIT_HAL_H

#include "hal/hal.h"

#include "test/unit/controller.h"
#include "test/unit/testable.h"

#include "lib/utils.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class TestHAL : public hal::HAL, public Testable, public Controller
			{
				typedef hal::HAL::Field Field;
				typedef hal::HAL::Event Event;

				public:
					TestHAL( );

					uint32_t in(Field) override;
					void out(Field, uint32_t) override;
					void set(Field f, bitmask_t v) override { out(f, getField(f) | v); }
					void reset(Field f, bitmask_t v) override { out(f, getField(f) & ~v); }

					void clear( ) override;

					const data_t& reads( ) const override { return mReads; }
					const data_t& writes( ) const override { return mWrites; }

					void setField(Field f, uint32_t v) override { mBuffer[static_cast<uint>(f)] = v; }
					uint32_t getField(Field f) const override { return mBuffer[static_cast<uint>(f)]; }

					void setCallback(callback_fn f) override { mCallback = f; }
					void trigger(Event) override;

				private:
					void resetFields( );

				private:
					data_t mReads, mWrites;
					uint32_t mBuffer[hal::HAL::N_FIELDS];
					callback_fn mCallback;
			};
		}
	}
}

#endif
