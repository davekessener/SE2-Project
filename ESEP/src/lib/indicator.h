#ifndef ESEP_LIB_INDICATOR_H
#define ESEP_LIB_INDICATOR_H

#include "lib/deferred.h"

namespace esep
{
	namespace lib
	{
		class Indicator
		{
			public:
				template<typename T>
					Indicator(T& o)
						: mCleanup([&o](void) { o = false; })
						  	 { o = true; }
			private:
				Deferred mCleanup;
		};
	}
}

#endif
