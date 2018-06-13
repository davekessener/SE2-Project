#include <lib/analyse/clustering.h>
#include "lib/stream/stream.h"

#include "lib/analyse/expand.h"

#include "lib/analyse/utils/low_pass.h"

#define MXT_THRESHOLD (((::esep::data::HeightMap::height_t)-1)*3/4)
#define MXT_FILTER_N 5
#define MXT_FILTER_M 5

namespace esep { namespace analyse {

Clustering::result_type Clustering::operator()(const data::HeightMap *hm)
{
	typedef lib::stream::Stream<in_type> stream_t;
	typedef Expand<in_type> exp_t;
	typedef utils::LowPass<double, MXT_FILTER_N> lp_t;

	result_type r;

	auto i1 = hm->begin(), i2 = hm->end(), i = i2;

	auto cull = [](const in_type& v) { return v.second < MXT_THRESHOLD; };

	for(; i1 != i2 && cull(*i1)  ; ++i1  );
	for(; i1 != i2 && cull(*--i) ; i2 = i);

	if(i1 != i2)
	{
		data::HeightMap::time_t max = 0;

		for(i = i1 ; i != i2 ; ++i)
		{
			if(i->first > max) max = i->first;
		}

		stream_t s0{exp_t{stream_t{i1, i2}}, exp_t{}};

		auto s1 = s0.map([max](const in_type& v) { return out_type(v.first / (double) max, v.second / (double) ((data::HeightMap::height_t)-1)); });

		auto s_x = s1.map([](const out_type& v) { return v[0];  })
				     .map([](double x) { return (x - 0.5) * 0.75 + 0.5; });
		auto s_y = s1.map([](const out_type& v) { return v[1]; });

		for(uint i = 0 ; i < MXT_FILTER_M ; ++i)
		{
			double initial = s_y.remove();
			for(uint i = 0 ; i < lp_t::SAMPLES + 1 ; ++i) s_y.push_front(initial);
			s_y = s_y.map(lp_t{});
			for(uint i = 0 ; i < lp_t::SAMPLES ; ++i) s_y.remove();
		}

		double y_max = (decltype(s_y){s_y}).select([](double a, double b) { return b > a; });

		s_y
		   .map([y_max](double y) { return y / y_max; })
		   .map([](double y) { return (y - 0.5) * 2; })
		   .map([](double y) { return (y < 0.0) ? 0.0 : ((y > 1.0) ? 1.0 : y); })
		   .collect([&](double y) { r.emplace_back(s_x.remove(), y); });
	}

	return r;
}

}}
