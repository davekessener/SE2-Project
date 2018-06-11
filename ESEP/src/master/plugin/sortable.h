#ifndef ESEP_MASTER_PLUGIN_SORTABLE_H
#define ESEP_MASTER_PLUGIN_SORTABLE_H

#include "master/plugin.h"

namespace esep
{
	namespace master
	{
		namespace plugin
		{
			template<Plugin::Type ... T>
			class Sortable : public virtual Plugin
			{
				public:
					Action decide(const history_t& h) override
					{
						return Action::KEEP;
					}
			};
		}
	}
}

#endif
