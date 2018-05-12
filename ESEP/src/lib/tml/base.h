#ifndef ESEP_TML_BASE_H
#define ESEP_TML_BASE_H

#include <stdint.h>

#include "lib/tml/if.h"

namespace esep
{
	namespace tml
	{
		typedef decltype(nullptr) Nil;

		struct Null { };

		template<typename T>
		using Car = typename T::Head;

		template<typename T>
		using Cdr = typename T::Tail;

		struct Identity
		{
			template<typename T>
			struct Apply
			{
				typedef T Type;
			};
		};

		template<typename T>
		struct Decay
		{
			typedef T Type;
		};

		template<typename T> struct Decay<const T> : Decay<T> { };
		template<typename T> struct Decay<T&> : Decay<T> { };
		template<typename T> struct Decay<T&&> : Decay<T> { };
		template<typename T> struct Decay<volatile T> : Decay<T> { };
		template<typename T, size_t N> struct Decay<T (&)[N]> : Decay<T *> { };

		template<typename T>
		using DoDecay = typename Decay<T>::Type;

		template<typename T>
		struct MinVal
		{
			static constexpr T s_min = 1ull << (sizeof(T) * 8 - 1);
			static constexpr T u_min = 0;
			static constexpr T Value = (s_min < u_min) ? s_min : u_min;
		};

		template<typename T>
		struct MaxVal
		{
			static constexpr T s_max = (1ull << (sizeof(T) * 8 - 1)) - 1;
			static constexpr T u_max = -1;
			static constexpr T Value = (s_max > u_max) ? s_max : u_max;
		};

		struct SmallType { char v[1]; };
		struct LargeType { char v[2]; };

		template<typename T, T V>
		struct Val2Type
		{
			static constexpr T Value = V;
		};

		template<int I>
		struct Int2Type : Val2Type<int, I>
		{
		};

		template<size_t I>
		struct UInt2Type : Val2Type<size_t, I>
		{
		};

		template<typename T>
		struct Type2Type
		{
			typedef T Type;
		};

		template<typename F, typename T>
		struct CanApply
		{
			template<typename FF, typename = typename FF::template Apply<T>>
			static SmallType f(FF&&);

			static LargeType f(...);

			static constexpr bool Value = sizeof(f(std::declval<F>())) == sizeof(SmallType);
		};

		template<typename T>
		struct IsValueType
		{
			template<typename TT>
			using Apply = decltype(TT::Value);

			static constexpr bool Value = CanApply<IsValueType<T>, T>::Value;
		};

		template<typename T>
		struct IsTypeType
		{
			template<typename TT>
			using Apply = typename TT::Type;

			static constexpr bool Value = CanApply<IsTypeType<T>, T>::Value;
		};

		template<typename T1, typename T2>
		struct IsExplicitlyCastable
		{
			template<typename TT>
			using Apply = decltype(static_cast<T2>(std::declval<TT>()));

			static constexpr bool Value = CanApply<IsExplicitlyCastable<T1, T2>, T1>::Value;
		};

		template<typename T1, typename T2>
		struct IsImplicitlyCastable
		{
			static SmallType f(T2);
			static LargeType f(...);

			static constexpr bool Value = sizeof(f(std::declval<T1&>())) == sizeof(SmallType);
		};

		template<template <typename ...> class F>
		struct Fun2Type
		{
			template<bool V, typename T>
			struct impl
			{
				typedef T Type;
			};

			template<typename T>
			struct impl<true, T> : T
			{
			};

			template<typename ... T>
			struct Apply
			{
				typedef F<T...> result_type;

				typedef typename impl<
					IsTypeType<result_type>::Value,
					result_type
				>::Type Type;
			};
		};

		template<template <typename ...> class F, typename T>
		struct CanApply<Fun2Type<F>, T>
		{
			struct impl
			{
				template<typename TT>
				using Apply = F<TT>;
			};

			static constexpr bool Value = CanApply<impl, T>::Value;
		};

		template<typename L>
		struct IsList
		{
			static constexpr bool Value = CanApply<Fun2Type<Car>, L>::Value && CanApply<Fun2Type<Cdr>, L>::Value;
		};

		template<typename T>
		struct SizeOf
		{
			static constexpr size_t Value = sizeof(T);
		};
	}
}

#endif
