#ifndef ESEP_ANALYSE_PREPROCESSOR_H
#define ESEP_ANALYSE_PREPROCESSOR_H

#include <vector>
#include <type_traits>

#include "lib/analyse/utils/low_pass.h"
#include "lib/analyse/utils/interpolation.h"
#include "lib/analyse/utils/trim.h"

#include "lib/stream/stream.h"
#include "lib/stream/range.h"

#include "data/heightmap_data.h"

namespace esep
{
	namespace analyse
	{
		MXT_DEFINE_E(InvalidProfileException);

		template<typename T, typename C = std::vector<T>>
		class Preprocessor
		{
			public:
			typedef T out_type;
			typedef typename impl::ValTraits<tml::IsPrimitive<T>::Value, T>::value_type primitive_t;
			typedef C container_type;
			typedef data::HeightMap::value_type in_type;
			typedef data::HeightMap::time_t time_t;
			typedef data::HeightMap::height_t height_t;

			template<typename Iterator>
			using IsRandomAccessIterator = std::is_base_of<
				std::random_access_iterator_tag,
				typename std::iterator_traits<Iterator>::iterator_category>;

			template<typename TT>
			using Stream = lib::stream::Stream<TT>;

			static constexpr primitive_t NORMALIZE = static_cast<height_t>(~0);
			static constexpr primitive_t LOWPASS_DEFAULT = 0.75;
			static constexpr size_t SEGMENTS = 100;
			static constexpr size_t N_LOWPASS = 2;
			static constexpr size_t M_LOWPASS = 2;

			public:
				container_type operator()(const data::HeightMap *hm)
				{
					container_type r;

					process(r, hm->begin(), hm->end());

					return r;
				}

			private:
				template<typename I, typename std::enable_if<!IsRandomAccessIterator<I>::value>::type * = nullptr>
				void process(container_type& r, I&& i1, I&& i2)
				{
					std::vector<decltype(*i1)> tmp(std::forward<I>(i1), std::forward<I>(i2));

					process(r, std::begin(tmp), std::end(tmp));
				}

				template<typename I, typename std::enable_if<IsRandomAccessIterator<I>::value>::type * = nullptr>
				void process(container_type& r, I&& i1, I&& i2)
				{
					typedef utils::LowPass<primitive_t, N_LOWPASS> low_pass_t;
					typedef utils::Interpolation<out_type> inter_t;

					time_t time_max = 0, time_min = 0;
					size_t n = 0;

					utils::trim(i1, i2, 0.85, 10);

					if(i1 == i2)
					{
						MXT_THROW_EX(InvalidProfileException);
					}

					r.reserve(SEGMENTS);

					time_min = i1->first;
					for(auto i = i1 ; i != i2 ; ++i, ++n)
					{
						if(i->first > time_max) time_max = i->first;
					}

					auto transform = [time_min, time_max](const in_type& v) -> out_type {
						return {(v.first - time_min) / (primitive_t) (time_max - time_min), v.second / NORMALIZE};
					};

					auto get = [&](size_t i) { return transform(i1[i]); };

					inter_t value_of(n, get, utils::inter::Linear<primitive_t>{});

					primitive_t q = SEGMENTS - 1;
					auto abcdef = SEGMENTS;

					auto s = (Stream<uint>::from<lib::stream::Range<uint>>(0, abcdef))
						.map([&](uint i) -> primitive_t { return i / q; })
						.map([&](primitive_t x) -> out_type { return out_type(x, value_of(x)); });

					struct Filter
					{
						Filter( ) : f(LOWPASS_DEFAULT) { }

						out_type operator()(const out_type& v) { return out_type(v.x(), f(v.y())); }

						low_pass_t f;
					};

					for(uint i = 0 ; i < M_LOWPASS ; ++i)
					{
						s = s.map(Filter{});
					}

					s.collect([&](const out_type& v) { r.push_back(v); });
				}

		};
	}
}

#endif
