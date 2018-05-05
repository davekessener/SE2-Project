#ifndef ESEP_TML_BASE_H
#define ESEP_TML_BASE_H

#include <type_traits>

namespace esep
{
	namespace tml
	{
		typedef decltype(nullptr) Nil;

		template<typename T>
		struct Decay
		{
			typedef typename std::decay<T>::type Type;
		};

		template<typename T>
		using DoDecay = typename Decay<T>::Type;

		template<typename T1, typename T2>
		struct TypeAssoc
		{
			typedef T1 Key;
			typedef T2 Value;
		};

		template<int I>
		struct Int2Type
		{
			static constexpr int Value = I;
		};

		template<typename T>
		struct Type2Type
		{
			typedef T Type;
		};

		template<typename ... T>
		struct Void
		{
			typedef void Type;
		};

		template<typename ... T>
		using ToVoid = typename Void<T...>::Type;

		// # -------------------------------------------------------------------------

		template<typename B>
		struct Not
		{
			static constexpr bool Value = !B::Value;
		};

		template<typename ... B>
		struct And
		{
			static constexpr bool Value = true;
		};

		template<typename B, typename ... A>
		struct And<B, A...>
		{
			static constexpr bool Value = B::Value && And<A...>::Value;
		};

		template<typename ... B>
		struct Or
		{
			static constexpr bool Value = false;
		};

		template<typename B, typename ... A>
		struct Or<B, A...>
		{
			static constexpr bool Value = B::Value || Or<A...>::Value;
		};
	}
}

#endif
