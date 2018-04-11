#ifndef ESEP_TML_FUNCTION_H
#define ESEP_TML_FUNCTION_H

#include "lib/utils.h"
#include "lib/tml.h"

namespace esep
{
	namespace lib
	{
		template<typename T>
		struct FunctionTraits : public FunctionTraits<decltype(&T::operator())>
		{
		};

		template<typename R, typename C, typename ... A>
		struct FunctionTraits<R(C::*)(A...) const>
		{
			static const uint arg_count = sizeof...(A);
			typedef R return_type;
			typedef C class_type;
			typedef tml::MakeTypeList<A...> argument_types;
			static constexpr bool IsMember = true;
		};

		template<typename R, typename ... A>
		struct FunctionTraits<R(*)(A...)>
		{
			static const uint arg_count = sizeof...(A);
			typedef R return_type;
			typedef tml::Nil class_type;
			typedef tml::MakeTypeList<A...> argument_types;
			static constexpr bool IsMember = false;
		};
	}
}

#endif
