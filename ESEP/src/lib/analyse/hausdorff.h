#ifndef ESEP_ANALYSE_HAUSDORFF_H
#define ESEP_ANALYSE_HAUSDORFF_H

#include <random>
#include <limits>
#include <algorithm>

#include "lib/analyse/algorithm.h"

namespace esep
{
	namespace analyse
	{
		namespace hausdorff
		{
			template<typename R = std::mt19937>
			struct Randomizer
			{
				template<typename I>
				void randomize(I&& i1, I&& i2) const
				{
					R g;

					std::shuffle(i1, i2, g);
				}
			};

			template<typename T>
			struct SquaredDistance
			{
				auto distance(const T& a, const T& b) const -> decltype(std::declval<T&>().abs())
				{
					return (a - b).abs_sqr();
				}
			};
		}
		template
		<
			typename T,
			typename R = hausdorff::Randomizer<>,
			typename D = hausdorff::SquaredDistance<T>,
			typename C = std::vector<T>
		>
		class Hausdorff : private R, private D
		{
			typedef T value_type;
			typedef typename impl::ValTraits<tml::IsPrimitive<T>::Value, T>::value_type primitive_t;
			typedef C container_type;

			public:
				template<typename I>
				Hausdorff(I&& i1, I&& i2) : mProfile(i1, i2)
				{
					R::randomize(std::begin(mProfile), std::end(mProfile));
				}
				template<typename CC> explicit Hausdorff(const CC& c) : Hausdorff(std::begin(c), std::end(c)) { }

				template<typename CC>
				primitive_t distance(const CC& cmp) const
				{
					container_type c(std::begin(cmp), std::end(cmp));

					R::randomize(std::begin(c), std::end(c));

					auto d1 = directional_impl(mProfile, c);
					auto d2 = directional_impl(c, mProfile);

					return d1 > d2 ? d1 : d2;
				}

				template<typename CC>
				primitive_t directional(const CC& cmp) const
				{
					container_type c(std::begin(cmp), std::end(cmp));

					randomize(c);

					return directional_impl(mProfile, c);
				}

			private:
				primitive_t directional_impl(const container_type& c1, const container_type& c2) const
				{
					typedef std::numeric_limits<primitive_t> limits_t;

					primitive_t cmin, cmax;

					cmax = limits_t::min();

					for(const auto& a : c1)
					{
						cmin = limits_t::max();

						for(const auto& b : c2)
						{
							auto d = D::distance(a, b);

							if(d < cmax)
							{
								cmin = limits_t::min();
								break;
							}

							if(d < cmin)
							{
								cmin = d;
							}
						}

						if(cmin > cmax)
						{
							cmax = cmin;
						}
					}

					return cmax;
				}

			private:
				container_type mProfile;
		};
	}
}

#endif
