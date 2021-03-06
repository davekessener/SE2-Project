#include "lib/analyse/profiles.h"

#include "lib/logger.h"

#define MXT_SAMPLES 100
#define MXT_PROFILES 11

#define MXT_TOP 0.825
#define MXT_BOTTOM 0.11
#define MXT_HIGH 0.7
#define MXT_LOW 0.55
#define MXT_FLAT 0.62


namespace esep { namespace analyse {

namespace
{
	typedef std::vector<Profiles::profile_t> map_t;
	typedef std::function<double(double)> fn;

	Profiles::profile_t sample(fn f, size_t n = MXT_SAMPLES)
	{
		Profiles::profile_t r;
		double q = n - 1;

		r.reserve(n);

		for(uint i = 0 ; i < n ; ++i)
		{
			double x = i / q;

			r.emplace_back(x, f(x));
		}

		return r;
	}

	map_t generate( )
	{
		typedef Profiles::Item Item;

		map_t r(MXT_PROFILES);

		auto assign = [&](Item i, fn f) { r.at(static_cast<uint>(i)) = sample(f); };

		auto coded = [](uint code) -> fn {
			return [=](double x) -> double {
				x = (x >= 0.5 ? 1 - x : x) * 2;

				uint s = x * 6.5;

				switch(s)
				{
				case 0:
				case 2:
				case 4:
					return MXT_TOP;

				case 1:
				case 3:
				case 5:
				case 6:
					return (code & (1 << (2 - (s - 1) / 2))) ? MXT_HIGH : MXT_LOW;

				default:
					MXT_LOG_FATAL("This should never be encountered!");
					std::terminate();
				}
			};
		};

		assign(Item::FLAT, [](double x) { return MXT_FLAT; });
		assign(Item::UPSIDEDOWN, [](double x) { return MXT_TOP; });
		assign(Item::HOLLOW, [](double x) { return (x >= 0.33 && x <= 0.66) ? MXT_BOTTOM : MXT_TOP; });
		assign(Item::CODED_000, coded(7));
		assign(Item::CODED_001, coded(6));
		assign(Item::CODED_010, coded(5));
		assign(Item::CODED_011, coded(4));
		assign(Item::CODED_100, coded(3));
		assign(Item::CODED_101, coded(2));
		assign(Item::CODED_110, coded(1));
		assign(Item::CODED_111, coded(0));

		return r;
	}
}

const Profiles::profile_t& Profiles::get(Item i)
{
	static map_t profiles = generate();

	return profiles.at(static_cast<uint>(i));
}

}}
