#ifndef ESEP_MASTER_PLUGIN_CONSTANT_H
#define ESEP_MASTER_PLUGIN_CONSTANT_H

#include "master/plugin.h"

namespace esep
{
	namespace master
	{
		namespace plugin
		{
			template<Plugin::Action A>
			class Constant : public virtual Plugin
			{
				public:
				static constexpr Action ACTION = A;

				public:
					Constant(Type t) : Plugin(t) { }

					Action decide(const history_t&) override { return ACTION; }
			};
		}
	}
}

#endif
