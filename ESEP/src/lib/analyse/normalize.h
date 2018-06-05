#ifndef ESEP_ANALYSE_NORMALIZE_H
#define ESEP_ANALYSE_NORMALIZE_H

#include <vector>

#include "data/heightmap_data.h"

namespace esep
{
	namespace analyse
	{
		template
		<
			typename T,
			typename C = std::vector<T>
		>
		class Normalize
		{
			typedef T value_type;
			typedef C container_type;

			public:
				container_type operator()(const data::HeightMap *hm)
				{
					container_type r;
					data::HeightMap::time_t max = 0;
					size_t l = 0;

					for(const auto& e : *hm)
					{
						if(e.first > max) max = e.first;

						++l;
					}

					double f = max;

					r.reserve(l);

					for(const auto& e : *hm)
					{
#define TO_F(v) ((v) / (double) (1 << (sizeof(v) * 8)))
						r.emplace_back(e.first / f, MXT_TO_F(e.second));
#undef TO_F
					}

					return r;
				}
		};
	}
}

#endif
