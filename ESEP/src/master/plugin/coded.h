#ifndef ESEP_MASTER_PLUGIN_CODED_H
#define ESEP_MASTER_PLUGIN_CODED_H

#include "master/plugin.h"
#include "master/plugin/hausdorff.h"
#include "master/plugin/constant.h"

#include "lib/analyse/profiles.h"

#include "lib/tml.h"

namespace esep
{
	namespace master
	{
		namespace plugin
		{
			template<Plugin::Type T, Plugin::Action A>
			class Coded : public Hausdorff, public Constant<A>
			{
				static constexpr Profile PROFILE = LookupProfile<T>::Value;

				public:
					Coded(processor_t *p) : Plugin(T), Hausdorff(p, T, PROFILE), Constant<A>(T) { }
			};
		}
	}
}

#endif
