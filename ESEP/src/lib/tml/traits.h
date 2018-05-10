#ifndef ESEP_TML_TRAITS_H
#define ESEP_TML_TRAITS_H

namespace esep
{
	namespace tml
	{
		template<template <typename> class F, typename T>
		struct CanApply
		{
			struct impl
			{
				struct small { char v[1]; };
				struct large { char v[2]; };

				template<typename TT, typename = F<TT>>
				static small f(TT&&);

				static large f(...);

				static constexpr bool Value = sizeof(f(std::declval<T>())) == sizeof(small);
			};

			static constexpr bool Value = impl::Value;
		};

		template<typename T1, typename T2>
		struct CanCastTo
		{
			template<typename TT>
			using impl = decltype(static_cast<T2>(std::declval<TT>()));

			static constexpr bool Value = CanApply<impl, T1>::Value;
		};
	}
}

#endif
