#ifndef ESEP_MASTER_PLUGIN_DEFAULT_H
#define ESEP_MASTER_PLUGIN_DEFAULT_H

#include "master/plugin.h"

namespace esep
{
	namespace master
	{
		namespace plugin
		{
			class Default : public Plugin
			{
				public:
					Default( ) : Plugin(Type::DEFAULT) { }
					float match(const data_t&) override { return 1.0f; }
					Action decide(const history_t&) override { return Action::KEEP; }
			};
		}
	}
}

#endif
