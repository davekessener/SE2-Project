#ifndef ESEP_TML_BASE_H
#define ESEP_TML_BASE_H

#include <type_traits>

namespace esep
{
	namespace tml
	{
		typedef decltype(nullptr) Nil;

		template<typename T>
		struct Decay
		{
			typedef typename std::decay<T>::type Type;
		};

		template<typename T>
		using DoDecay = typename Decay<T>::Type;
	}
}

#endif
