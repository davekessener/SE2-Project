#ifndef ESEP_TML_APPLY_H
#define ESEP_TML_APPLY_H

#include "lib/tml/type_list.h"

namespace esep
{
	namespace tml
	{
		template<typename L, template<typename> class F>
		struct Apply;

		template<typename H, typename T, template<typename> class F>
		struct Apply<TypeList<H, T>, F>
		{
			template<typename TT> struct Gimme;

			typedef typename F<H>::Type Element;
			typedef typename Apply<T, F>::Type Tail;
			typedef TypeList<Element, Tail> Type;
		};

		template<template <typename> class F>
		struct Apply<Nil, F>
		{
			typedef Nil Type;
		};

		template<typename L, template <typename> class F>
		using DoApply = typename Apply<L, F>::Type;
	}
}

#endif
