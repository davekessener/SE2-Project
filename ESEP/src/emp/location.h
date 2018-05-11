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

				static Location byName(const std::string&);
				static Location byHW(Field, uint32_t);
				static Location byEvent(Event);

				bool operator==(const Location& l) const { return field == l.field && pin == l.pin; }
				bool operator!=(const Location& l) const { return !(*this == l); }

			private:
				Location(const std::string& s, Field f, uint32_t p)
					: name(s), field(f), pin(p)
				{ }

				static Location findLocation(predicate_fn);
		};
	}
}

#endif
