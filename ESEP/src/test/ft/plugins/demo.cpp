#include <lib/analyse/clustering.h>
#include "test/ft/plugins/demo.h"

#include "lib/utils.h"
#include "lib/timer.h"
#include "lib/processor.h"
#include "lib/process_tree.h"
#include "lib/writer.h"

#include "lib/io/file_writer.h"

#include "lib/analyse/bdscan.h"

#include "data/heightmap_data.h"

#include "hal.h"
#include "hal/physical.h"

#include "master/plugin/hausdorff_item.h"
#include "master/plugin/sortable.h"

#include "lib/utils/storage.h"
#include "lib/logger.h"

#include "lib/analyse/hausdorff.h"
#include "lib/analyse/profiles.h"

namespace esep { namespace test { namespace functional { namespace plugins {

namespace
{
	enum class State
	{
		IDLE,
		RUNNING,
		IN_HS,
		RETURNING,
		READY
	};

	typedef analyse::Profiles::Item ItemProfile;
	typedef master::Plugin::Type PluginType;

	struct SamplePlugin : public master::plugin::Hausdorff
	{
		typedef master::plugin::Hausdorff::processor_t processor_t;

		SamplePlugin(PluginType t, processor_t *p, ItemProfile i) : Plugin(t), Hausdorff(p, t, i) { }

		Action decide(const history_t&) override { return Action::KEEP; }
	};

	struct FlatItemPlugin : SamplePlugin
	{
		typedef std::function<bool(double)> pred_fn;

		FlatItemPlugin(PluginType t, processor_t *p, pred_fn f)
			: Plugin(t)
		    , SamplePlugin(t, p, ItemProfile::FLAT)
		 	, mCheck(f) { }

		float match(const data_t& d)
		{
			float f = SamplePlugin::match(d);
			double y = 0;

			for(const auto& p : d)
			{
				if(p->type() == data::DataPoint::Type::HEIGHT_MAP)
				{
					const auto& data(use(dynamic_cast<data::HeightMap *>(p.get())).get<master::plugin::Hausdorff::Station::NORMALIZED>());

					for(uint i = 10 ; i < data.size() - 20 ; ++i)
					{
						y += data[i].y();
					}

					y /= data.size() - 20;
				}
			}

			return mCheck(y) ? f : 0;
		}

		pred_fn mCheck;
	};

	std::string ITEM_NAME(PluginType t)
	{
		switch(t)
		{
		case PluginType::FLAT:
			return "FLAT";

		case PluginType::UPSIDEDOWN:
			return "UPSIDE-DOWN";

		case PluginType::HOLLOW:
			return "HOLLOW";

		case PluginType::HOLLOW_METAL:
			return "HOLLOW (M)";

		case PluginType::CODED_000:
			return "CODED 000";

		case PluginType::CODED_001:
			return "CODED 001";

		case PluginType::CODED_010:
			return "CODED 010";

		case PluginType::CODED_011:
			return "CODED 011";

		case PluginType::CODED_100:
			return "CODED 100";

		case PluginType::CODED_101:
			return "CODED 101";

		case PluginType::CODED_110:
			return "CODED 110";

		case PluginType::CODED_111:
			return "CODED 111";

		case PluginType::UNKNOWN:
			return "UNKOWN";

		case PluginType::DEFAULT:
			return "DEFAULT";
		}

		throw t;
	}
}

Demo::Demo(void)
{
	HAL_CONSOLE.println("=== START OF PLUGIN TEST =======================================");
}

Demo::~Demo(void)
{
	HAL_CONSOLE.println("================================================================");
}

#define MXT_FLAT_UD_THRESH 0.63

void Demo::run(void)
{
	typedef hal::HAL::Event Event;
	typedef hal::LightBarriers::LightBarrier LB;
	typedef hal::Lights::Light Light;
	typedef hal::Buttons::Button Button;

	master::plugin::Hausdorff::processor_t processor;
	std::vector<std::unique_ptr<master::Plugin>> plugins;

	plugins.emplace_back(new FlatItemPlugin(PluginType::FLAT, &processor, [](double y) { return y < MXT_FLAT_UD_THRESH; }));
	plugins.emplace_back(new FlatItemPlugin(PluginType::UPSIDEDOWN, &processor, [](double y) { return y > MXT_FLAT_UD_THRESH; }));
	plugins.emplace_back(new SamplePlugin(PluginType::HOLLOW, &processor, ItemProfile::HOLLOW));
	plugins.emplace_back(new SamplePlugin(PluginType::CODED_101, &processor, ItemProfile::CODED_101));
	plugins.emplace_back(new SamplePlugin(PluginType::CODED_010, &processor, ItemProfile::CODED_010));
	plugins.emplace_back(new SamplePlugin(PluginType::CODED_110, &processor, ItemProfile::CODED_110));

	base::ConfigObject config;

	if(!config.isValid())
	{
		MXT_LOG_ERROR("No valid configuration! Abort!");
		return;
	}

	HAL::instance().clear();

	hal::Physical hal;

	HAL::instance().instantiate(&hal, &config);

	std::unique_ptr<data::HeightMap> hm;
	lib::Writer_ptr fout;

	auto do_analysis = [&](data::Data_ptr p) {
		std::vector<data::Data_ptr> d;

		d.push_back(p);
		float m = 0.0;
		PluginType t;

		for(auto& p : plugins)
		{
			float r = p->match(d);

			if(r > m)
			{
				m = r;
				t = p->type();
			}
		}

		HAL_CONSOLE.println("Found a ", ITEM_NAME(t), " item! (~ ", (int)(m * 100), "%)");
	};

	State state = State::IDLE;
	std::atomic<bool> running(true);
	timer::Manager cb;

	uint64_t ts = 0;
	uint count = 0;
	bool hs_done = false;

	HAL_LIGHTS.turnOn(Light::GREEN);

	hal.setCallback([&](Event e) {
		if(e == Event::BTN_STOP && HAL_BUTTONS.isPressed(Button::STOP))
		{
			running = false;
		}

		start:

		switch(state)
		{
		case State::IDLE:
			if(e == Event::LB_START && HAL_LIGHT_BARRIERS.isBroken(LB::LB_START))
			{
				HAL_MOTOR.right();
				HAL_MOTOR.start();
				HAL_LIGHTS.flash(Light::YELLOW, 500);
				state = State::RUNNING;
			}
			break;

		case State::RUNNING:
			if(e == Event::HEIGHT_SENSOR && HAL_HEIGHT_SENSOR.measure())
			{
				hm.reset(new data::HeightMap);
				fout.reset(new lib::FileWriter(lib::stringify("item_",  lib::hex<8>(count++), ".hm")));
				state = State::IN_HS;
				HAL_LIGHTS.turnOn(Light::RED);
				ts = lib::Timer::instance().elapsed();
				goto start;
			}
			break;

		case State::IN_HS:
		{
			if(e == Event::HEIGHT_SENSOR)
			{
				if(uint16_t v = HAL_HEIGHT_SENSOR.measure())
				{
					uint16_t t = lib::Timer::instance().elapsed() - ts;
					std::string l = lib::stringify(t, " ", v, ";");

					hm->addHeightValue(t, v);
					fout->writeLine(l);

					if(hs_done) cb.reset();
					hs_done = false;
				}
				else if(!hs_done)
				{
					hs_done = true;

					cb = lib::Timer::instance().registerAsync([&](void) {
						fout.reset();

						do_analysis(data::Data_ptr(hm.release()));

						processor.clear();

						HAL_MOTOR.left();
						HAL_LIGHTS.turnOff(Light::RED);

						state = State::RETURNING;
					}, 15);
				}
			}
		}
			break;

		case State::RETURNING:
			if(e == Event::LB_START && HAL_LIGHT_BARRIERS.isBroken(LB::LB_START))
			{
				HAL_MOTOR.stop();
				HAL_LIGHTS.turnOn(Light::YELLOW);
				state = State::READY;
			}
			break;

		case State::READY:
			if(e == Event::LB_START && !HAL_LIGHT_BARRIERS.isBroken(LB::LB_START))
			{
				HAL_LIGHTS.turnOff(Light::YELLOW);
				state = State::IDLE;
			}
			break;

		default:
			break;
		}
	});

	while(running.load())
	{
		lib::Timer::instance().sleep(10);
	}
}

}}}}
