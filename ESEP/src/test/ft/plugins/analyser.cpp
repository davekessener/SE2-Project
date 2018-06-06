#include "test/ft/plugins/analyser.h"

#include "lib/processor.h"
#include "lib/process_tree.h"

#include "lib/analyse/analyser.h"
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
			Tag<Station::NORMALIZED_MAP, analyse::Analyser>,
			Tag<Station::CLUSTER, analyse::BDSCAN<vec2>>>>
	process_t;
	typedef lib::Processor<data::HeightMap *, process_t> processor_t;

	processor_t p;
	data::HeightMap hm;

	for(uint i = 0 ; i < N_DATA() ; ++i)
	{
		hm.addHeightValue(RAW_DATA(i * 2), RAW_DATA(i * 2 + 1));
	}

	HAL_CONSOLE.println("Processing ", N_DATA(), " datapoints.");

	auto c = p.processor(&hm).get<Station::CLUSTER>();

	HAL_CONSOLE.println("Found ", c.size(), " cluster.");

	for(const auto& p : c)
	{
		HAL_CONSOLE.println(std::setprecision(17), p[0], " ", p[1]);
	}
}

}}}}
