#ifndef ESEP_TML_INDEXOF_H
#define ESEP_TML_INDEXOF_H

#include "lib/tml/type_list.h"

namespace esep
{
	namespace tml
	{
		template<typename L, typename T>
		struct IndexOf;

		template<typename H, typename T, typename TT>
		struct IndexOf<TypeList<H, T>, TT>
		{
			static const int tmp = IndexOf<T, TT>::Value;
			static const int Value = tmp >= 0 ? tmp + 1 : tmp;
		};

		template<typename H, typename T>
		struct IndexOf<TypeList<H, T>, H>
		{
			static const int Value = 0;
		};

		template<typename T>
		struct IndexOf<Nil, T>
		{
			static const int Value = -1;
		};
	}
}

#endif
