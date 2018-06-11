#include <lib/analyse/clustering.h>
#include "test/ft/plugins/analyser.h"

#include "lib/processor.h"
#include "lib/process_tree.h"

#include "lib/analyse/bdscan.h"

#include "hal.h"

namespace esep { namespace test { namespace functional { namespace plugins {

namespace
{
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
}

Analyser::Analyser(void)
{
	HAL_CONSOLE.println("=== START OF PLUGIN TEST =======================================");
}

Analyser::~Analyser(void)
{
	HAL_CONSOLE.println("================================================================");
}

void Analyser::run(void)
{
	typedef analyse::V<double, 2> vec2;
	typedef ProcessTree<data::HeightMap *,
		Source<
			Tag<Station::NORMALIZED_MAP, analyse::Clustering>,
			Tag<Station::CLUSTER, analyse::BDSCAN<vec2>>>>
	process_t;
//	typedef lib::Processor<data::HeightMap *, process_t> processor_t;

//	processor_t p;
	data::HeightMap hm;

	for(uint i = 0 ; i < N_DATA() ; ++i)
	{
		hm.addHeightValue(RAW_DATA(i * 2), RAW_DATA(i * 2 + 1));
	}

	HAL_CONSOLE.println("Processing ", N_DATA(), " datapoints.");

	auto c = (process_t{&hm}).get<Station::CLUSTER>();
//	auto c = p.processor(&hm).get<Station::CLUSTER>();

	HAL_CONSOLE.println("Found ", c.size(), " cluster.");

	std::vector<double> t;

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
}

}}}}
