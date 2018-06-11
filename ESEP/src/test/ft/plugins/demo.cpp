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

	enum class Station
	{
		NORMALIZED_MAP,
		CLUSTER
	};

	template<Station S, typename T>
	using Tag = lib::data::Tagger<Station>::Tag<S, T>;

	template<typename S, typename ... T>
	using Source = lib::data::Source<S, T...>;

	template<typename T1, typename T2>
	using ProcessTree = lib::data::ProcessTree<Station, T1, T2>;

	template<typename C>
	void process(C&& c)
	{
		std::vector<double> t;

		HAL_CONSOLE.println("Found ", c.size(), " cluster.");

		for(uint i = 0 ; i < (c.size() + 1) / 2 ; ++i)
		{
			double y = (c[i][1] + c[c.size() - i - 1][1]) / 2;

			if(y < 0.875)
			{
				t.push_back(y);
			}
		}

		double min = 1.0, max = 0.0;
		for(double y : t) { if(y < min) min = y; if(y > max) max = y; }
		for(double& y : t) { y = (y - min) / (max - min); }

		std::vector<int> r;
		for(uint i = 1 ; i < t.size() ; ++i) r.push_back(1.5 * (t[i] - t[i-1]));

		if(r.size() == 2)
		{
			switch((r[0] + 1) * 3 + (r[1] + 1))
			{
			case 0:
				HAL_CONSOLE.println("Invalid: ", r[0], " ", r[1], "!");
				break;
			case 1:
				HAL_CONSOLE.println("100");
				break;
			case 2:
				HAL_CONSOLE.println("101");
				break;
			case 3:
				HAL_CONSOLE.println("110");
				break;
			case 4:
				HAL_CONSOLE.println((min + max) / 2 >= 0.66 ? "000" : "111");
				break;
			case 5:
				HAL_CONSOLE.println("001");
				break;
			case 6:
				HAL_CONSOLE.println("010");
				break;
			case 7:
				HAL_CONSOLE.println("011");
				break;
			case 8:
				HAL_CONSOLE.println("Invalid: ", r[0], " ", r[1], "!");
				break;
			default:
				HAL_CONSOLE.println("Invalid: ", r[0], " ", r[1], "!");
				break;
			}
		}
		else
		{
			HAL_CONSOLE.println("Results inconclusive: ");
			for(auto e : r) HAL_CONSOLE.println(e);
		}

		HAL_CONSOLE.println("----------");
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

void Demo::run(void)
{
	typedef analyse::V<double, 2> vec2;
	typedef ProcessTree<data::HeightMap *,
		Source<
			Tag<Station::NORMALIZED_MAP, analyse::Clustering>,
			Tag<Station::CLUSTER, analyse::BDSCAN<vec2>>>>
	process_t;
//	typedef lib::Processor<data::HeightMap *, process_t> processor_t;

	typedef hal::HAL::Event Event;
	typedef hal::LightBarriers::LightBarrier LB;
	typedef hal::Lights::Light Light;
	typedef hal::Buttons::Button Button;

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

	State state = State::IDLE;
	std::atomic<bool> running(true);

	uint64_t ts = 0;

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
				fout.reset(new lib::FileWriter("item.hm"));
				state = State::IN_HS;
				HAL_LIGHTS.turnOn(Light::RED);
				ts = lib::Timer::instance().elapsed();
				goto start;
			}
			break;

		case State::IN_HS:
			if(uint16_t v = HAL_HEIGHT_SENSOR.measure())
			{
				uint16_t t = lib::Timer::instance().elapsed() - ts;
				hm->addHeightValue(t, v);
				fout->writeLine(lib::stringify(t, " ", lib::hex<16>(v)));
			}
			else
			{
				fout.reset();

				process((process_t{hm.get()}).get<Station::CLUSTER>());
//				process(p.processor(hm.get()).get<Station::CLUSTER>());

				HAL_MOTOR.left();
				HAL_LIGHTS.turnOff(Light::RED);

				state = State::RETURNING;
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
