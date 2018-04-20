#ifndef ESEP_TML_REFDETECT_H
#define ESEP_TML_REFDETECT_H

#include "lib/tml/base.h"

namespace esep
{
	namespace tml
	{
		template<typename T>
		struct IsLValue
		{
			struct Small { char v[1]; };
			struct Big { char v[2]; };

			static Small testForLValue(...);
			static Big testForLValue(T);

			static DoDecay<T>& valueFunction( );

			static constexpr bool Value = (sizeof(testForLValue(valueFunction())) == sizeof(Big));
		};

		template<typename T>
		constexpr bool is_lvalue(T&& o)
		{
			return IsLValue<decltype(o)>::Value;
		}
	}
}

#endif
