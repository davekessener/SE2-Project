#ifndef ESEP_TML_TYPELIST_H
#define ESEP_TML_TYPELIST_H

#include "lib/tml/base.h"

namespace esep
{
	namespace tml
	{
		template<typename H, typename T>
		struct TypeList
		{
			typedef H Head;
			typedef T Tail;
		};

		template<typename ... T>
		struct TypeListMaker;

		template<typename T, typename ... TT>
		struct TypeListMaker<T, TT...>
		{
			typedef TypeList<T, typename TypeListMaker<TT...>::Type> Type;
		};

		template<>
		struct TypeListMaker<>
		{
			typedef Nil Type;
		};

		template<typename ... T>
		using MakeTypeList = typename TypeListMaker<T...>::Type;
	}
}

#endif
