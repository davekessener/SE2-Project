#include <forward_list>
#include <algorithm>

#include "emp/location.h"

#include "lib/logger.h"

namespace esep { namespace emp {

typedef std::unique_lock<std::mutex> lock_t;

Location Location::byEvent(Event e)
{
	try
	{
		return byHW(hal::HAL::getField(e), hal::HAL::getPin(e));
	}
	catch(const UnknownLocationException& ex)
	{
		MXT_THROW_E(UnknownLocationException, "Unknown event ", e, "!");
	}
}

Location Location::byName(const std::string& n)
{
	try
	{
		return findLocation([&n](const Location& l) { return l.name == n; });
	}
	catch(const UnknownLocationException& e)
	{
		MXT_THROW_E(UnknownLocationException, "Unknown name '", n, "'!");
	}
}

Location Location::byHW(Field f, uint32_t p)
{
	try
	{
		return findLocation([f, p](const Location& l) { return l.field == f && l.pin == p; });
	}
	catch(const UnknownLocationException& e)
	{
		MXT_THROW_E(UnknownLocationException, "Unknown hw location ", f, ":$", lib::hex<32>(p), "!");
	}
}

Location Location::findLocation(predicate_fn f)
{
	static std::forward_list<Location> locations;
	static std::mutex mtx;

	if(locations.empty())
	{
		lock_t lock(mtx);

		if(locations.empty())
		{
			locations.push_front(Location("lb_start",            Field::GPIO_0, 1 <<  2));
			locations.push_front(Location("lb_heightsensor",     Field::GPIO_0, 1 <<  3));
			locations.push_front(Location("heightsensor_valid",  Field::GPIO_0, 1 <<  4));
			locations.push_front(Location("lb_switch",           Field::GPIO_0, 1 <<  5));
			locations.push_front(Location("metalsensor",         Field::GPIO_0, 1 <<  7));
			locations.push_front(Location("switch_in",           Field::GPIO_0, 1 << 14));
			locations.push_front(Location("lb_ramp",             Field::GPIO_0, 1 << 15));
			locations.push_front(Location("lb_end",              Field::GPIO_0, 1 << 20));
			locations.push_front(Location("button_start",       Field::GPIO_0, 1 << 22));
			locations.push_front(Location("button_stop",        Field::GPIO_0, 1 << 23));
			locations.push_front(Location("button_reset",       Field::GPIO_0, 1 << 26));
			locations.push_front(Location("button_estop",       Field::GPIO_0, 1 << 27));

			locations.push_front(Location("motor_right",   Field::GPIO_1, 1 << 12));
			locations.push_front(Location("motor_left",    Field::GPIO_1, 1 << 13));
			locations.push_front(Location("motor_slow",    Field::GPIO_1, 1 << 14));
			locations.push_front(Location("motor_disable", Field::GPIO_1, 1 << 15));
			locations.push_front(Location("switch_out",    Field::GPIO_1, 1 << 19));
			locations.push_front(Location("lights_red",    Field::GPIO_1, 1 << 16));
			locations.push_front(Location("lights_yellow", Field::GPIO_1, 1 << 17));
			locations.push_front(Location("lights_green",  Field::GPIO_1, 1 << 18));

			locations.push_front(Location("led_start", Field::GPIO_2, 1 << 2));
			locations.push_front(Location("led_reset", Field::GPIO_2, 1 << 3));
			locations.push_front(Location("led_q1",    Field::GPIO_2, 1 << 4));
			locations.push_front(Location("led_q1",    Field::GPIO_2, 1 << 5));

			locations.push_front(Location("heightsensor", Field::ANALOG, 0));
		}
	}

	auto i = std::find_if(locations.begin(), locations.end(), f);

	if(i == locations.end())
	{
		MXT_THROW_EX(UnknownLocationException);
	}
	else
	{
		return *i;
	}
}

}}
