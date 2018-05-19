#ifndef ESEP_TML_LOGIC_H
#define ESEP_TML_LOGIC_H

namespace esep
{
	namespace tml
	{
		struct True  { static constexpr bool Value = true;  };
		struct False { static constexpr bool Value = false; };

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
	}
}

#endif
