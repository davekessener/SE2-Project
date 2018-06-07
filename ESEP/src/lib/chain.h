#ifndef ESEP_LIB_CHAIN_H
#define ESEP_LIB_CHAIN_H

#include "lib/utils.h"
#include "lib/tml.h"
#include "lib/function.h"

namespace esep
{
	namespace lib
	{
		namespace impl
		{
			template<typename L>
			struct ChainApply;

			template<typename F, typename TT>
			struct ChainApply<tml::TypeList<F, TT>>
			{
				typedef FunctionTraits<F> traits_t;
				typedef typename traits_t::return_type return_type;
				typedef tml::DoDecay<tml::list::DoGet<typename traits_t::argument_types, 0>> argument_type;

				static return_type f(argument_type&& a)
				{
					auto v(ChainApply<TT>::f(std::move(a)));

					return F()(std::move(v));
				}
			};

			template<>
			struct ChainApply<tml::Nil>
			{
				template<typename T>
				static T f(T&& a)
				{
					return std::forward<T>(a);
				}
			};
		}

		template<typename First, typename ... Remainder>
		class FunChain
		{
			typedef FunctionTraits<First> first_traits;
			typedef tml::MakeTypeList<First, Remainder...> functor_types;
			typedef FunctionTraits<tml::list::GetLast<functor_types>> last_traits;
			typedef tml::DoDecay<tml::list::DoGet<typename first_traits::argument_types, 0>> argument_type;
			typedef typename last_traits::return_type return_type;


			public:
				return_type operator()(const argument_type& a)
				{
					return impl::ChainApply<tml::list::DoReverse<functor_types>>::f(argument_type(a));
				}
		};
	}
}

#endif
