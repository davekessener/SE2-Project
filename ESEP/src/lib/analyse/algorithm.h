#ifndef ESEP_ANALYSE_ALGORITHM_H
#define ESEP_ANALYSE_ALGORITHM_H

#include <cmath>

namespace esep
{
	namespace analyse
	{
		inline float sqrt(float x) { return std::sqrt(x); }
		inline double sqrt(double x) { return std::sqrt(x); }

		namespace impl
		{
			template<bool IsBasic, typename T> struct ValTraits { typedef typename T::value_type value_type; };
			template<typename T> struct ValTraits<true, T> { typedef T value_type; };
		}
	}
}

#endif
