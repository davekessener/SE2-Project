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

	// # -----------------------------------------------------------------

	namespace list
	{
		template
		<
			typename L,
			typename F
		>
		struct Apply
		{
			typedef TypeList<
				typename F::template Apply<Car<L>>::Type,
				typename Apply<Cdr<L>, F>::Type
			> Type;
		};

		template<typename F>
		struct Apply<Nil, F>
		{
			typedef Nil Type;
		};

		template<typename C, typename F>
		using DoApply = typename Apply<C, F>::Type;

	// # -----------------------------------------------------------------

		template
		<
			typename L,
			typename F,
			typename T = Nil
		>
		struct Collect
		{
			typedef typename F::template Apply<
				Car<L>,
				typename Collect<Cdr<L>, F, T>::Type
			>::Type Type;
		};

		template<typename F, typename T>
		struct Collect<Nil, F, T>
		{
			typedef T Type;
		};

		template<typename L, typename F, typename T = Nil>
		using DoCollect = typename Collect<L, F, T>::Type;

	// # -----------------------------------------------------------------

		template<typename L>
		struct And : DoCollect<L, Fun2Type<logic::And>, True>
		{
		};

		template<typename L>
		struct Or : DoCollect<L, Fun2Type<logic::Or>, False>
		{
		};

	// # -----------------------------------------------------------------

		template
		<
			typename L,
			typename F
		>
		struct All : And<DoApply<L, F>>
		{
		};

		template
		<
			typename L,
			typename F
		>
		struct Any : Or<DoApply<L, F>>
		{
		};

	// # -----------------------------------------------------------------

		template<typename L, typename T>
		struct Contains : Any<L, Bind<Fun2Type<Equals>, 0, T>>
		{
		};

	// # -----------------------------------------------------------------

		template<typename L>
		struct Max : DoCollect<
			L,
			Fun2Type<logic::Max>,
			MinVal<decltype(Car<L>::Value)>
		>
		{
		};

		template<typename L>
		struct Min : DoCollect<
			L,
			Fun2Type<logic::Max>,
			MaxVal<decltype(Car<L>::Value)>
		>
		{
		};

	// # -----------------------------------------------------------------

		template<typename L, typename T>
		struct IndexOf
		{
			static_assert(Contains<L, T>::Value, "Can't calculate index of element if it's not in the list!");

			template<bool F, typename LL, typename TT>
			struct impl
			{
				static constexpr size_t Value = 1 + impl<Equals<Car<LL>, TT>::Value, Cdr<LL>, TT>::Value;
			};

			template<typename LL, typename TT>
			struct impl<true, LL, TT>
			{
				static constexpr size_t Value = 0;
			};

			template<typename TT>
			struct impl<false, Nil, TT>
			{
			};

			static constexpr size_t Value = impl<false, L, T>::Value - 1;
		};

	// # -----------------------------------------------------------------

		template<typename L>
		struct Length : UInt2Type<1 + Length<Cdr<L>>::Value>
		{
		};

		template<>
		struct Length<Nil> : UInt2Type<0>
		{
		};

	// # -----------------------------------------------------------------

		template<typename L, size_t N>
		struct Get : Get<Cdr<L>, N - 1>
		{
			static_assert(Length<L>::Value > N, "Can't get value beyond list length!");
		};

		template<typename L>
		struct Get<L, 0>
		{
			static_assert(!Equals<L, Nil>::Value, "Can't get value beyond list length!");

			typedef Car<L> Type;
		};

		template<typename L, size_t N>
		using DoGet = typename Get<L, N>::Type;
	}
}}

#endif
