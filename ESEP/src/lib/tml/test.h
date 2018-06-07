#ifndef ESEP_TML_TEST_H
#define ESEP_TML_TEST_H

#include "lib/tml/base.h"
#include "lib/tml/algorithms.h"
#include "lib/tml/if.h"
#include "lib/tml/logic.h"
#include "lib/tml/ref_detect.h"
#include "lib/tml/str.h"
#include "lib/tml/type_list.h"

namespace esep {
namespace tml_test
{
	using namespace tml;

	enum class TestEnum { };

	namespace test_types
	{
		static_assert(MinVal< int8_t>::Value == -128, "");
		static_assert(MinVal<uint8_t>::Value == 0, "");
		static_assert(MaxVal< int8_t>::Value == 127, "");
		static_assert(MaxVal<uint8_t>::Value == 255, "");

		static_assert(sizeof(SmallType) != sizeof(LargeType), "");

		static_assert(Val2Type<int, 413>::Value == 413, "");
		static_assert(Equals<Type2Type<int>::Type, int>::Value, "");

		static_assert( IsValueType<True>::Value, "");
		static_assert( IsValueType<False>::Value, "");
		static_assert(!IsValueType<void>::Value, "");
		static_assert(!IsValueType<Type2Type<void>>::Value, "");

		static_assert(!IsTypeType<void>::Value, "");
		static_assert( IsTypeType<Type2Type<void>>::Value, "");

		static_assert( IsExplicitlyCastable<char, long>::Value, "");
		static_assert( IsExplicitlyCastable<TestEnum, int>::Value, "");
		static_assert(!IsExplicitlyCastable<char, std::string>::Value, "");

		static_assert( IsImplicitlyCastable<char, long>::Value, "");
		static_assert(!IsImplicitlyCastable<TestEnum, int>::Value, "");

		static_assert(Equals<DoDecay<int>,          int>::Value, "");
		static_assert(Equals<DoDecay<int&>,         int>::Value, "");
		static_assert(Equals<DoDecay<int&&>,        int>::Value, "");
		static_assert(Equals<DoDecay<const int>,    int>::Value, "");
		static_assert(Equals<DoDecay<const int&>,   int>::Value, "");
		static_assert(Equals<DoDecay<volatile int>, int>::Value, "");
		static_assert(Equals<DoDecay<int *>,        int *>::Value, "");
		static_assert(Equals<DoDecay<int (&)[1]>,   int *>::Value, "");
		static_assert(Equals<DoDecay<int * const>,  int *>::Value, "");
	}

	namespace logic_test
	{
		using namespace logic;

		static_assert(True::Value, "");
		static_assert(!False::Value, "");

		static_assert( Equals<int, int>::Value, "");
		static_assert(!Equals<int, unsigned int>::Value, "");
		static_assert(!Equals<int, void *>::Value, "");
		static_assert( Equals<Nil, decltype(nullptr)>::Value, "");

		static_assert( Not<False>::Value, "");
		static_assert(!Not<True>::Value, "");

		static_assert(!And<False, False>::Value, "");
		static_assert(!And<True,  False>::Value, "");
		static_assert(!And<False,  True>::Value, "");
		static_assert( And<True,   True>::Value, "");

		static_assert(!Or<False, False>::Value, "");
		static_assert( Or<True,  False>::Value, "");
		static_assert( Or<False,  True>::Value, "");
		static_assert( Or<True,   True>::Value, "");
	}

	namespace list_test
	{
		using namespace list;

		typedef TypeList<int, TypeList<void *, TypeList<double, Nil>>> ref_t;
		typedef MakeTypeList<int, void *, double> list_t;

		static_assert(Equals<list_t, ref_t>::Value, "");

		static_assert( Contains<list_t, int>::Value, "");
		static_assert(!Contains<list_t, char>::Value, "");

		static_assert(IndexOf<list_t, int>::Value    == 0, "");
		static_assert(IndexOf<list_t, void *>::Value == 1, "");
		static_assert(IndexOf<list_t, double>::Value == 2, "");

		static_assert( IsList<list_t>::Value, "");
		static_assert(!IsList<Type2Type<void>>::Value, "");
		static_assert(!IsList<void>::Value, "");

		static_assert(Equals<DoGet<list_t, 0>, int>::Value, "");
		static_assert(Equals<DoGet<list_t, 1>, void *>::Value, "");
		static_assert(Equals<DoGet<list_t, 2>, double>::Value, "");

		static_assert(Length<list_t>::Value == 3, "");

		typedef MakeTypeList<UInt2Type<sizeof(int)>, UInt2Type<sizeof(void *)>, UInt2Type<sizeof(double)>> size_list_t;
		typedef DoApply<list_t, Fun2Type<SizeOf>> apply_list_t;

		static_assert(DoGet<apply_list_t, 0>::Value == DoGet<size_list_t, 0>::Value, "");
		static_assert(DoGet<apply_list_t, 1>::Value == DoGet<size_list_t, 1>::Value, "");
		static_assert(DoGet<apply_list_t, 2>::Value == DoGet<size_list_t, 2>::Value, "");

		static_assert(Equals<DoAppend<MakeTypeList<int, double>, void>, MakeTypeList<int, double, void>>::Value, "");
		static_assert(Equals<DoAppend<MakeTypeList<int, double>, MakeTypeList<void, char>>, MakeTypeList<int, double, void, char>>::Value, "");

		static_assert(Equals<DoFlatten<MakeTypeList<void>>, MakeTypeList<void>>::Value, "");
		static_assert(Equals<DoFlatten<MakeTypeList<
			MakeTypeList<int, MakeTypeList<
				char, double>,
				MakeTypeList<void>>,
				float *>>,
			MakeTypeList<int, char, double, void, float *>>::Value, "");

		template<size_t V>
		struct HasSize
		{
			template<typename T>
			struct Apply
			{
				typedef Bool2Type<sizeof(T) == V> Type;
			};
		};

		static_assert(Equals<DoFindFirst<MakeTypeList<long, int, short, char, float, double>, HasSize<1>>, char>::Value, "");
		static_assert(Equals<DoReverse<MakeTypeList<char, short, int, long>>, MakeTypeList<long, int, short, char>>::Value, "");
	}

	namespace if_test
	{
		static_assert(Equals<void *, DoIf<True,  Type2Type<void *>, Type2Type<int>>>::Value, "");
		static_assert(Equals<int,    DoIf<False, Type2Type<void *>, Type2Type<int>>>::Value, "");

		template<typename T>
		EnableIf<Equals<T, int>, std::string> test_enable( );

		template<typename T>
		EnableIf<Not<Equals<T, int>>, char> test_enable( );

		static_assert(Equals<std::string, decltype(test_enable<int   >())>::Value, "");
		static_assert(Equals<char,        decltype(test_enable<void *>())>::Value, "");

		typedef DoIf<True, Type2Type<void>, void> does_not_eval_else;
	}

	namespace str_test
	{
		using namespace string;

		static_assert( CanPrint<int>::Value, "");
		static_assert( CanPrint<std::string>::Value, "");
		static_assert( CanPrint<TestEnum>::Value, "");
		static_assert( CanPrint<std::pair<void *, int>>::Value, "");
		static_assert(!CanPrint<std::pair<Null, int>>::Value, "");
		static_assert(!CanPrint<void>::Value, "");
	}
}}

#endif
