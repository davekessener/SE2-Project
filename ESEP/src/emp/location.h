#ifndef ESEP_EMP_LOCATION_H
#define ESEP_EMP_LOCATION_H

#include <functional>

#include "lib/utils.h"

#include "hal/HAL.h"

namespace esep
{
	namespace emp
	{
		class Location
		{
			typedef hal::HAL::Field Field;
			typedef hal::HAL::Event Event;
			typedef std::function<bool(const Location&)> predicate_fn;

			public:
			MXT_DEFINE_E(UnknownLocationException);

			public:
				const std::string name;
				const Field field;
				const uint32_t pin;

				static Location byName(const std::string& n)
					{ return findLocation([&n](const Location& l) { return l.name == n; }); }
				static Location byHW(Field f, uint32_t p)
					{ return findLocation([f, p](const Location& l) { return l.field == f && l.pin == p; }); }
				static Location byEvent(Event);

			private:
				Location(const std::string&, Field, uint32_t);

				static Location findLocation(predicate_fn);
		};
	}
}

#endif
