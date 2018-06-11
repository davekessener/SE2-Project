#ifndef ESEP_TEST_UT_PLUGINS_H
#define ESEP_TEST_UT_PLUGINS_H

#include <vector>
#include <memory>

#include "test/unit/unit.h"

#include "master/plugin.h"
#include "master/plugin/hausdorff_item.h"

namespace esep
{
	namespace test
	{
		namespace ut
		{
			class Plugins : public unit::TestSuite
			{
				typedef master::Plugin::Type PluginType;
				typedef master::plugin::Hausdorff::processor_t Processor;

				public:
					Plugins( );

				protected:
					void setup( ) override;
					void teardown( ) override;
					void define( ) override;

				private:
					std::unique_ptr<Processor> mProcessor;
					std::vector<std::unique_ptr<master::Plugin>> mPlugins;
			};
		}
	}
}

#endif
