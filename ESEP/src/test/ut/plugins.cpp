#include "test/ut/plugins.h"

#include "test/unit/assertions.h"

#include "master/plugin/hausdorff_item.h"
#include "master/plugin/sortable.h"

#include "lib/utils/storage.h"
#include "lib/logger.h"

#include "lib/analyse/hausdorff.h"
#include "lib/analyse/profiles.h"

namespace esep { namespace test { namespace ut {

namespace
{
	typedef analyse::Profiles::Item ItemProfile;

	struct SamplePlugin : public master::plugin::Hausdorff
	{
		typedef master::Plugin::Type Type;
		typedef master::plugin::Hausdorff::processor_t processor_t;

		SamplePlugin(Type t, processor_t *p, ItemProfile i) : Plugin(t), Hausdorff(p, t, i) { }

		Action decide(const history_t&) override { return Action::KEEP; }
	};
}

Plugins::Plugins(void)
	: unit::TestSuite("Plugins")
{
}

void Plugins::setup(void)
{
	mProcessor.reset(new Processor);

	mPlugins.emplace_back(new SamplePlugin(PluginType::FLAT, mProcessor.get(), ItemProfile::FLAT));
	mPlugins.emplace_back(new SamplePlugin(PluginType::HOLLOW, mProcessor.get(), ItemProfile::HOLLOW));
	mPlugins.emplace_back(new SamplePlugin(PluginType::CODED_101, mProcessor.get(), ItemProfile::CODED_101));
	mPlugins.emplace_back(new SamplePlugin(PluginType::CODED_010, mProcessor.get(), ItemProfile::CODED_010));
	mPlugins.emplace_back(new SamplePlugin(PluginType::CODED_110, mProcessor.get(), ItemProfile::CODED_110));
}

void Plugins::teardown(void)
{
	mProcessor.reset();
	mPlugins.clear();
}

void Plugins::define(void)
{
	typedef std::pair<PluginType, data::Data_ptr> data_t;

	UNIT_TEST("can identify one")
	{
		std::vector<data::Data_ptr> d;

		d.push_back(lib::utils::Storage::instance().retrieve("item_flat_1").as<data_t>().second);

		float m = 0.0;
		PluginType t;

		for(auto& p : mPlugins)
		{
			float r = p->match(d);

			if(r > m)
			{
				m = r;
				t = p->type();
			}
		}

		ASSERT_EQUALS(t, PluginType::FLAT);
	};

	UNIT_TEST("can identify all")
	{
		std::vector<std::string> names;

		names.push_back("item_flat_1");
		names.push_back("item_hollow_1");
		names.push_back("item_hollow_metal_1");
		names.push_back("item_new_bin_010_1");
		names.push_back("item_new_bin_101_1");
		names.push_back("item_new_bin_110_1");
		names.push_back("item_old_bin_010_1");
		names.push_back("item_old_bin_101_1");
		names.push_back("item_old_bin_110_1");
		names.push_back("item_flat_2");
		names.push_back("item_hollow_2");
		names.push_back("item_hollow_metal_2");
		names.push_back("item_new_bin_010_2");
		names.push_back("item_new_bin_101_2");
		names.push_back("item_new_bin_110_2");
		names.push_back("item_old_bin_010_2");
		names.push_back("item_old_bin_101_2");
		names.push_back("item_old_bin_110_2");

		auto find = [this](data::Data_ptr d) {
			std::vector<data::Data_ptr> a;
			float m = 0;
			PluginType t;

			a.push_back(d);

			for(auto& p : mPlugins)
			{
				float r = p->match(a);

				if(r > m)
				{
					m = r;
					t = p->type();
				}
			}

			return t;
		};

		for(const std::string& n : names)
		{
			data_t d = lib::utils::Storage::instance().retrieve(n).as<data_t>();

			ASSERT_EQUALS(find(d.second), d.first);
		}
	};
}

}}}
