#include "test/ut/plugins.h"

#include "test/unit/assertions.h"

#include "master/plugin/hausdorff_item.h"
#include "master/plugin/sortable.h"

namespace esep { namespace test { namespace ut {

namespace
{
	struct SamplePlugin : public master::plugin::Hausdorff
	{
		typedef master::Plugin::Type Type;
		typedef master::plugin::Hausdorff::processor_t processor_t;
		typedef analyse::Profiles::Item profile_t;

		SamplePlugin(Type t, processor_t *p, profile_t i) : Hausdorff(p, t, i) { }

		Action decide(const history&) override { return Action::KEEP; }
	};
}

Plugins::Plugins(void)
	: unit::TestSuite("Plugins")
{
}

void Plugin::setup(void)
{
}

void Plugin::teardown(void)
{
	mProcessor.reset();
	mPlugins.clear();
}

void Plugins::define(void)
{
	UNIT_TEST("")
	{
	};
}

}}}
