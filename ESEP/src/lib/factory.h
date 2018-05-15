#ifndef ESEP_LIB_FACTORY_H
#define ESEP_LIB_FACTORY_H

#include "lib/utils.h"
#include "lib/tml.h"
#include "lib/function.h"

namespace esep
{
	namespace lib
	{
		template<typename T1, T1 V1, typename T2, T2 V2>
		struct Generator
		{
			typedef T1 key_type;
			typedef T2 generator_type;
			static constexpr T1 key = V1;
			static constexpr T2 generator = V2;
		};

		MXT_DEFINE_E(UnknownKeyException);

		namespace factory_impl
		{
			template<typename L>
			struct DefaultComparison
			{
				typedef typename tml::Car<L>::key_type key_type;
				bool operator()(const key_type& v1, const key_type& v2) { return v1 == v2; }
			};

			template<typename L, typename F, typename E, typename V>
			struct FactoryImpl
			{
				typedef L type_map;
				typedef typename tml::Car<type_map>::key_type key_type;
				typedef typename tml::Car<type_map>::generator_type generator_type;
				typedef FunctionTraits<generator_type> generator_traits;
				typedef typename generator_traits::return_type value_type;
				typedef FactoryImpl<tml::Cdr<L>, F, E, V> sub_factory;
				static constexpr key_type KEY = tml::Car<type_map>::key;
				static constexpr generator_type GENERATOR = tml::Car<type_map>::generator;

				template<typename ... A>
				static value_type produce(const key_type& key, A&& ... args)
				{
					return F()(key, KEY) ? GENERATOR(std::forward<A>(args)...) : sub_factory::produce(key, args...);
				}
			};

			template<typename F, typename E, typename V>
			struct FactoryImpl<tml::Nil, F, E, V>
			{
				template<typename ... A>
				static V produce(A&& ...)
				{
					throw E();
				}
			};
		}

		template
		<
			typename L,
			typename F = factory_impl::DefaultComparison<L>,
			typename E = UnknownKeyException
		>
		struct Factory : public factory_impl::FactoryImpl<L, F, E,
			typename FunctionTraits<typename tml::Car<L>::generator_type>::return_type>
		{
		};
	}
}

#endif
