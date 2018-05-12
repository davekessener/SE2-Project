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
			static SmallType testForLValue(...);
			static LargeType testForLValue(T);

			static constexpr bool Value = (sizeof(testForLValue(std::declval<DoDecay<T>&>())) == sizeof(LargeType));
		};

		template<typename T>
		constexpr bool is_lvalue(T&& o)
		{
			return IsLValue<decltype(o)>::Value;
		}
	}
}

#endif
