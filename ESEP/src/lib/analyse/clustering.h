#ifndef ESEP_ANALYSE_CLUSTERING_H
#define ESEP_ANALYSE_CLUSTERING_H

#include <vector>

#include "data/heightmap_data.h"

#include "lib/analyse/v.h"

namespace esep
{
	namespace analyse
	{
		class Clustering
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
