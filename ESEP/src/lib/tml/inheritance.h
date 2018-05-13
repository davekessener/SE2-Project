#ifndef ESEP_TML_INHERITANCE_H
#define ESEP_TML_INHERITANCE_H

#include "lib/tml/base.h"
#include "lib/tml/type_list.h"

namespace esep
{
	namespace tml
	{
		template<typename L>
		struct ScatterHierarchy : ScatterHierarchy<Cdr<L>>, Car<L>
		{
		};

		template<>
		struct ScatterHierarchy<Nil>
		{
		};

		template<typename ... T>
		using GenScatterHierarchy = ScatterHierarchy<MakeTypeList<T...>>;

		template<typename L>
		struct LinearHierarchy : DoCall<Car<L>, LinearHierarchy<Cdr<L>>>
		{
		};

		template<>
		struct LinearHierarchy<Nil>
		{
		};

		template<typename ... T>
		using GenLinearHierarchy = LinearHierarchy<MakeTypeList<T...>>;
	}
}

#endif
