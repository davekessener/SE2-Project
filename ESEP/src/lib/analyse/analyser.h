#ifndef ESEP_ANALYSE_ANALYSE_H
#define ESEP_ANALYSE_ANALYSE_H

#include <vector>

#include "data/heightmap_data.h"

#include "lib/analyse/v.h"

namespace esep
{
	namespace analyse
	{
		class Analyser
		{
			typedef data::HeightMap::value_type in_type;
			typedef V<double, 2> out_type;
			typedef std::vector<out_type> result_type;

			public:
				result_type operator()(const data::HeightMap *);
		};
	}
}

#endif
