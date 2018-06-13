#ifndef ESEP_ANALYSE_PROFILES_H
#define ESEP_ANALYSE_PROFILES_H

#include <vector>

#include "lib/analyse/v.h"

namespace esep
{
	namespace analyse
	{
		class Profiles
		{
			public:
			enum class Item
			{
				FLAT,
				HOLLOW,
				UPSIDEDOWN,
				CODED_000,
				CODED_001,
				CODED_010,
				CODED_011,
				CODED_100,
				CODED_101,
				CODED_110,
				CODED_111
			};

			typedef std::vector<vec2> profile_t;

			static const profile_t& get(Item);
		};
	}
}

#endif
