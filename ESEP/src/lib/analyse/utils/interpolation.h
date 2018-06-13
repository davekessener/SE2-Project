#ifndef ESEP_ANALYSE_UTILS_INTERPOLATION_H
#define ESEP_ANALYSE_UTILS_INTERPOLATION_H

namespace esep
{
	namespace analyse
	{
		namespace utils
		{
			namespace inter
			{
				template<typename T>
				struct Linear
				{
					T operator()(size_t i, T x, std::function<T(size_t)> f, size_t n) const
					{
						int i1 = i - 1, i2 = i;

						return f(i1) * (1 - x) + f(i2) * x;
					}
				};
			}

			template<typename T>
			class Interpolation
			{
				typedef typename impl::ValTraits<tml::IsPrimitive<T>::Value, T>::value_type primitive_t;
				typedef std::function<T(size_t)> get_fn;
				typedef std::function<primitive_t(size_t, primitive_t, std::function<primitive_t(size_t)>, size_t)> inter_fn;

				public:
					Interpolation(size_t n, get_fn f, inter_fn i) : n(n), mGet(f), mInter(i) { }

					primitive_t operator()(primitive_t x) const
					{
						size_t j = 0;

						for(size_t i = 0 ; i < n ; ++i, ++j)
						{
							if(mGet(i).x() > x) break;
						}

						if(j == 0) return mGet(0).y();
						if(j == n) return mGet(n-1).y();

						return mInter(j, x - mGet(j).x(), [this](size_t i) { return mGet(i).y(); }, n);
					}

				private:
					const size_t n;
					get_fn mGet;
					inter_fn mInter;
			};
		}
	}
}

#endif
