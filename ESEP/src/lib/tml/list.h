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
			struct Flatten : Flatten<R, Cdr<L>, T..., Car<L>>
			{
			};

			template<typename R, typename ... T>
			struct Flatten<R, Nil, T...> : Call<R, T...>
			{
			};

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
		}
	}
}

#endif
