#ifndef ESEP_TML_LIST_H
#define ESEP_TML_LIST_H

#include "lib/tml/algorithms.h"

namespace esep
{
	namespace tml
	{
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

			template<typename L> struct Last;
			template<typename H, typename T> struct Last<TypeList<H, T>> : Last<T> { };
			template<typename T> struct Last<TypeList<T, Nil>> { typedef T Type; };

			template<typename L>
			using GetLast = typename Last<L>::Type;

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

			template<typename L, typename F>
			struct FindFirst
			{
				template<bool, typename H, typename T>
				struct Impl : Impl<DoCall<F, Car<T>>::Value, Car<T>, Cdr<T>>
				{
				};

				template<typename H, typename T>
				struct Impl<true, H, T>
				{
					typedef H Type;
				};

				template<typename H>
				struct Impl<false, H, Nil>
				{
//					static_assert(false, "Not found in list!");
				};

				typedef typename Impl<false, Nil, L>::Type Type;
			};

			template<typename L, typename F>
			using DoFindFirst = typename FindFirst<L, F>::Type;

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
// # -----------------------------------------------------------------

			template<typename L, size_t I>
			struct IsValidIndex
			{
				static constexpr bool Value = I < Length<L>::Value;
			};

// # -----------------------------------------------------------------

			template
			<
				typename R,
				typename L,
				typename ... T
			>
			struct ToVariadic : ToVariadic<R, Cdr<L>, T..., Car<L>>
			{
			};

			template<typename R, typename ... T>
			struct ToVariadic<R, Nil, T...> : Call<R, T...>
			{
			};

// # -----------------------------------------------------------------

			template<typename L, typename T>
			struct Append
			{
				typedef TypeList<Car<L>, typename Append<Cdr<L>, T>::Type> Type;
			};

			template<typename T>
			struct Append<Nil, T>
			{
				typedef TypeList<T, Nil> Type;
			};

			template<typename L>
			struct Append<L, Nil>
			{
				typedef L Type;
			};

			template<>
			struct Append<Nil, Nil>
			{
				typedef Nil Type;
			};

			template<typename H, typename T>
			struct Append<Nil, TypeList<H, T>>
			{
				typedef TypeList<H, T> Type;
			};

			template<typename L, typename T>
			using DoAppend = typename Append<L, T>::Type;

// # -----------------------------------------------------------------

			template<typename T>
			struct Flatten
			{
				typedef TypeList<T, Nil> Type;
			};

			template<>
			struct Flatten<Nil>
			{
				typedef Nil Type;
			};

			template<typename H, typename T>
			struct Flatten<TypeList<H, T>> : Append<typename Flatten<H>::Type, typename Flatten<T>::Type>
			{
			};

			template<typename T>
			using DoFlatten = typename Flatten<T>::Type;

// # -----------------------------------------------------------------

			template
			<
				typename L,
				typename F
			>
			struct ApplyWithIndex
			{
				template<typename LL, size_t I>
				struct impl
				{
					typedef TypeList<
						DoCall<
							F,
							Car<LL>,
							UInt2Type<I>>,
						typename impl<Cdr<LL>, I + 1>::Type>
					Type;
				};

				template<size_t I>
				struct impl<Nil, I>
				{
					typedef Nil Type;
				};

				typedef typename impl<L, 0>::Type Type;
			};

			template<typename L, typename F>
			using DoApplyWithIndex = typename ApplyWithIndex<L, F>::Type;

// # -----------------------------------------------------------------

			template<typename T>
			struct Reverse
			{
				template<typename L, typename R>
				struct Impl : Impl<Cdr<L>, TypeList<Car<L>, R>>
				{
				};

				template<typename R>
				struct Impl<Nil, R>
				{
					typedef R Type;
				};

				typedef typename Impl<T, Nil>::Type Type;
			};

			template<typename L>
			using DoReverse = typename Reverse<L>::Type;
		}
	}
}

#endif
