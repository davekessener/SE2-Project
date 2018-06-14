#include <master/plugin/hausdorff.h>
#include "test/ut/plugins.h"

#include "test/unit/assertions.h"

#include "master/plugin/items.h"

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

	mPlugins.emplace_back(new master::plugin::Coded_000(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::Coded_001(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::Coded_010(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::Coded_011(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::Coded_100(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::Coded_101(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::Coded_110(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::Coded_111(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::Flat(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::UpsideDown(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::Hollow(mProcessor.get()));
	mPlugins.emplace_back(new master::plugin::HollowMetal(mProcessor.get()));

//	mPlugins.emplace_back(new SamplePlugin(PluginType::FLAT, mProcessor.get(), ItemProfile::FLAT));
//	mPlugins.emplace_back(new SamplePlugin(PluginType::HOLLOW, mProcessor.get(), ItemProfile::HOLLOW));
//	mPlugins.emplace_back(new SamplePlugin(PluginType::CODED_101, mProcessor.get(), ItemProfile::CODED_101));
//	mPlugins.emplace_back(new SamplePlugin(PluginType::CODED_010, mProcessor.get(), ItemProfile::CODED_010));
//	mPlugins.emplace_back(new SamplePlugin(PluginType::CODED_110, mProcessor.get(), ItemProfile::CODED_110));
}

void Plugins::teardown(void)
{
	mPlugins.clear();
	mProcessor.reset();
}

void Plugins::define(void)
{
	typedef std::pair<PluginType, data::Data_ptr> data_t;
	typedef master::Plugin::Type Type;
	typedef master::Plugin::Action Action;
	typedef std::vector<Type> History;

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

	UNIT_TEST("Can sort")
	{
		History h{
			Type::CODED_000,
			Type::FLAT,
			Type::HOLLOW_METAL,
			Type::CODED_010,
			Type::UPSIDEDOWN
		};

		ASSERT_EQUALS(mPlugins[0]->decide(h),  Action::KEEP);
		ASSERT_EQUALS(mPlugins[1]->decide(h),  Action::TOSS_M);
		ASSERT_EQUALS(mPlugins[2]->decide(h),  Action::TOSS_S);
		ASSERT_EQUALS(mPlugins[3]->decide(h),  Action::KEEP);
		ASSERT_EQUALS(mPlugins[4]->decide(h),  Action::TOSS_M);
		ASSERT_EQUALS(mPlugins[5]->decide(h),  Action::KEEP);
		ASSERT_EQUALS(mPlugins[6]->decide(h),  Action::KEEP);
		ASSERT_EQUALS(mPlugins[7]->decide(h),  Action::TOSS_S);
		ASSERT_EQUALS(mPlugins[8]->decide(h),  Action::TOSS_M);
		ASSERT_EQUALS(mPlugins[9]->decide(h),  Action::TOSS);
		ASSERT_EQUALS(mPlugins[10]->decide(h), Action::KEEP);
		ASSERT_EQUALS(mPlugins[11]->decide(h), Action::KEEP);
	};
}

}}}
