#include "test/ut/hw_location.h"

#include "test/unit/assertions.h"

#include "emp/location.h"

namespace esep { namespace test { namespace ut {

typedef hal::HAL HAL;
typedef HAL::Field Field;
typedef hal::HAL::Event Event;

HWLocation::HWLocation(void)
	: TestSuite("HW Location")
{
}

void HWLocation::define(void)
{
	UNIT_TEST("can find by name")
	{
		auto l = emp::Location::byName("lb_start");

		ASSERT_EQUALS(l.name, "lb_start");
		ASSERT_EQUALS(l.field, Field::GPIO_0);
		ASSERT_EQUALS(l.pin, HAL::getPin(Event::LB_START));
	};

	UNIT_TEST("can find by event")
	{
		auto l = emp::Location::byEvent(hal::HAL::Event::LB_START);

		ASSERT_EQUALS(l.name, "lb_start");
		ASSERT_EQUALS(l.field, Field::GPIO_0);
		ASSERT_EQUALS(l.pin, HAL::getPin(Event::LB_START));
	};

	UNIT_TEST("can find by pin")
	{
		auto l = emp::Location::byHW(Field::GPIO_0, HAL::getPin(Event::LB_START));

		ASSERT_EQUALS(l.name, "lb_start");
		ASSERT_EQUALS(l.field, Field::GPIO_0);
		ASSERT_EQUALS(l.pin, HAL::getPin(Event::LB_START));
	};

	UNIT_TEST("reports error on invalid location")
	{
		ASSERT_FAILURE(emp::Location::byName(""), emp::Location::UnknownLocationException);
	};

	UNIT_TEST("represents all HAL events")
	{
		for(auto i1 = hal::HAL::EVENTS, i2 = i1 + hal::HAL::N_EVENTS ; i1 != i2 ; ++i1)
		{
			auto l = emp::Location::byEvent(*i1);
			auto f = hal::HAL::getField(*i1);
			auto p = hal::HAL::getPin(*i1);

			ASSERT_EQUALS(l.field, f);
			ASSERT_EQUALS(l.pin, p);
			ASSERT_TRUE(l == emp::Location::byHW(f, p));
		}
	};
}

}}}
