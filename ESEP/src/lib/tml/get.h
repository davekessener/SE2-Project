#ifndef ESEP_TML_GET_H
#define ESEP_TML_GET_H

#include "lib/tml/type_list.h"

namespace esep
{
	namespace tml
	{
		template<typename L, size_t I>
		struct Get;

		template<typename H, typename T, size_t I>
		struct Get<TypeList<H, T>, I>
		{
			typedef typename Get<T, I - 1>::Type Type;
		};

		template<typename H, typename T>
		struct Get<TypeList<H, T>, 0>
		{
			typedef H Type;
		};

		template<size_t I>
		struct Get<Nil, I>
		{
			typedef Nil Type;
		};

		template<typename L, size_t I>
		using DoGet = typename Get<L, I>::Type;
	}
}

#endif
