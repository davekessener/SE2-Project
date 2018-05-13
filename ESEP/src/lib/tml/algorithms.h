#ifndef ESEP_TML_ALGORITHMS_H
#define ESEP_TML_ALGORITHMS_H

#include "lib/tml/base.h"
#include "lib/tml/logic.h"
#include "lib/tml/type_list.h"

namespace esep
{
	namespace tml
	{
		template<template <typename...> class F, typename ... T>
		using Defer = typename Fun2Type<F>::template Apply<T...>::Type;

// # -----------------------------------------------------------------

	template
	<
		typename F,
		size_t N,
		typename T
	>
	struct Bind;

	template<typename F, typename T>
	struct Bind<F, 0, T>
	{
		template<typename ... TT>
		struct Apply : F::template Apply<T, TT...>
		{
		};
	};

	template<typename F, typename T>
	struct Bind<F, 1, T>
	{
		template<typename T1, typename ... TT>
		struct Apply : F::template Apply<T1, T, TT...>
		{
		};
	};

	template<typename F, typename T>
	struct Bind<F, 2, T>
	{
		template<typename T1, typename T2, typename ... TT>
		struct Apply : F::template Apply<T1, T2, T, TT...>
		{
		};
	};

	template<typename F, typename T>
	struct Bind<F, 3, T>
	{
		template<typename T1, typename T2, typename T3, typename ... TT>
		struct Apply : F::template Apply<T1, T2, T3, T, TT...>
		{
		};
	};
}}

#endif
