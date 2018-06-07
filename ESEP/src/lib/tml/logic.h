#ifndef ESEP_TML_LOGIC_H
#define ESEP_TML_LOGIC_H

namespace esep
{
	namespace tml
	{
		struct True  : Bool2Type<true>  { typedef True  Type; };
		struct False : Bool2Type<false> { typedef False Type; };

		template<typename T> struct IsConst : False { };
		template<typename T> struct IsConst<const T> : True { };

		template<typename T> struct IsVolatile : False { };
		template<typename T> struct IsVolatile<volatile T> : True { };

		template<typename T> struct IsPointer : False { };
		template<typename T> struct IsPointer<T *> : True { };

		template<typename> struct IsIntegral : False { };
		template<> struct IsIntegral<char> : True { };
		template<> struct IsIntegral<unsigned char> : True { };
		template<> struct IsIntegral<short> : True { };
		template<> struct IsIntegral<unsigned short> : True { };
		template<> struct IsIntegral<int> : True { };
		template<> struct IsIntegral<unsigned int> : True { };
		template<> struct IsIntegral<long> : True { };
		template<> struct IsIntegral<unsigned long> : True { };
		template<> struct IsIntegral<long long> : True { };
		template<> struct IsIntegral<unsigned long long> : True { };

		template<typename T> struct IsNumeric : IsIntegral<T> { };
		template<> struct IsNumeric<float> : True { };
		template<> struct IsNumeric<double> : True { };

		template<typename T1, typename T2>
		struct Promotion
		{
			typedef decltype(std::declval<T1>() + std::declval<T2>()) Type;
		};

		template<typename T1, typename T2>
		using DoPromotion = typename Promotion<T1, T2>::Type;

		namespace impl
		{
			template<typename T> struct IsPrimitive : False { };
		}

		template<typename T>
		struct IsUnsigned
		{
			static constexpr bool Value = ((T) -1) > ((T) 0);
		};

		template<typename T>
		struct Not
		{
			static constexpr bool Value = !T::Value;
		};

		template<typename T1, typename T2>
		struct Equals
		{
			static constexpr bool Value = false;
		};

		template<typename T>
		struct Equals<T, T>
		{
			static constexpr bool Value = true;
		};

		namespace logic
		{
			template<typename T1, typename T2>
			struct GreaterThan
			{
				static constexpr bool Value = T1::Value > T2::Value;
			};

			template<typename T1, typename T2>
			struct LessThan
			{
				static constexpr bool Value = T1::Value < T2::Value;
			};

			template<typename T1, typename T2>
			struct GreaterThanOrEqualTo
			{
				static constexpr bool Value = T1::Value >= T2::Value;
			};

			template<typename T1, typename T2>
			struct LessThanOrEqualTo
			{
				static constexpr bool Value = T1::Value <= T2::Value;
			};

			template<typename T1, typename T2>
			struct And
			{
				static constexpr bool Value = T1::Value && T2::Value;
			};

			template<typename T1, typename T2>
			struct Or
			{
				static constexpr bool Value = T1::Value || T2::Value;
			};

			template<typename T1, typename T2>
			struct Min
			{
#define MXT_MIN(a,b) (((a)<(b))?(a):(b))
				static constexpr decltype(T1::Value) Value = MXT_MIN(T1::Value, T2::Value);
#undef MXT_MIN
			};

			template<typename T1, typename T2>
			struct Max
			{
#define MXT_MAX(a,b) (((a)>(b))?(a):(b))
				static constexpr decltype(T1::Value) Value = MXT_MAX(T1::Value, T2::Value);
#undef MXT_MAX
			};
		}

		template<typename T> struct IsPrimitive : logic::Or<IsPointer<T>, IsNumeric<T>> { };
	}
}

#endif
