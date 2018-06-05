#ifndef ESEP_ANALYSE_BDSCAN
#define ESEP_ANALYSE_BDSCAN

#include <vector>

namespace esep
{
	namespace analyse
	{
		namespace impl
		{
			template<bool IsBasic, typename T> struct ValTraits { typedef typename T::value_type value_type; };
			template<typename T> struct ValTraits<true, T> { typedef T value_type; };
		}

		template
		<
			typename T,
			typename C = std::vector<T>
		>
		class BDSCAN
		{
			typedef T value_type;
			typedef C container_type;
			typedef uint8_t cid_t;
			typedef uint16_t idx_t;
			typedef typename impl::ValTraits<tml::IsPrimitive<T>::Value, T>::value_type primitive_t;

			static constexpr id_t VISITED = 0xFF;

			public:
				BDSCAN(primitive_t e, size_t n) : mEpsilon(e * e), mNeighborhood(n) { }

				container_type operator()(const container_type& a)
				{
					std::vector<cid_t> map(a.size());
					int next_id = 0;

					idx_t i = 0;
					for(auto& e : map)
					{
						if(!e)
						{
							auto neighbors = find_neighbors(a, a[i]);

							if(neighbors.size() >= mNeighborhood)
							{
								process_cluster(map, a, neighbors, ++next_id);
							}
							else
							{
								e = VISITED;
							}
						}

						++i;
					}

					container_type r;

					r.reserve(next_id);

					for(idx_t i = 1 ; i <= next_id ; ++i)
					{
						value_type v;
						uint n = 0;

						for(idx_t j = 0 ; j < a.size() ; ++j)
						{
							if(map[j] == i)
							{
								v += a[j];
								++n;
							}
						}

						v /= n;

						r.emplace_back(std::move(v));
					}

					return r;
				}

			private:
				std::vector<idx_t> find_neighbors(const container_type& data, const value_type& p)
				{
					std::vector<idx_t> r;

					idx_t i = 0;
					for(const auto& e : data)
					{
						if((e - p).abs_sqr() <= mEpsilon)
						{
							r.push_back(i);
						}

						++i;
					}

					return r;
				}

				void process_cluster(std::vector<cid_t>& map, const container_type& a, const std::vector<idx_t>& neighbors, cid_t id)
				{
					std::vector<idx_t> leftover;

					for(const auto& i : neighbors)
					{
						if(map[i] == 0 || map[i] == VISITED)
						{
							leftover.emplace_back(i);
							map[i] = id;
						}
					}

					for(const auto& i : leftover)
					{
						process_cluster(map, a, find_neighbors(a, a[i]), id);
					}
				}

			private:
				const primitive_t mEpsilon;
				const size_t mNeighborhood;
		};
	}
}

#endif
