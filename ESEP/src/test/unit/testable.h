#ifndef ESEP_TEST_UNIT_TESTABLE_H
#define ESEP_TEST_UNIT_TESTABLE_H

#include <functional>
#include <vector>

#include "lib/tuple.h"

#include "hal/hal.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class Testable
			{
				protected:
				typedef hal::HAL::Field Field;
				typedef hal::HAL::Event Event;
				typedef std::function<void(Event)> callback_fn;
				typedef lib::MakeTuple<uint64_t, Field, uint32_t> entry_t;
				typedef std::vector<entry_t> data_t;

				public:
					virtual ~Testable( ) { }

					virtual const data_t& reads( ) const = 0;
					virtual const data_t& writes( ) const = 0;

					virtual void setField(Field, uint32_t) = 0;
					virtual uint32_t getField(Field) const = 0;

					virtual void setCallback(callback_fn) = 0;
					virtual void trigger(Event) = 0;
			};
		}
	}
}

#endif
