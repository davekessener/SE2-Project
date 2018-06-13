#ifndef ESEP_ANALYSE_LOWPASS_H
#define ESEP_ANALYSE_LOWPASS_H

#include <array>

#include "lib/utils.h"

namespace esep
{
	namespace analyse
	{
		namespace utils
		{
			template<typename T, size_t N>
			class LowPass
			{
				public:
				static constexpr size_t SAMPLES = N;

				public:
					LowPass(T initial = 0) : mIdx(0), mDamp(calcDamp()) { for(uint i = 0 ; i < N ; ++i) mBuf[i] = initial; }
					T operator()(const T& v)
					{
						T r = 0;

						mBuf[mIdx] = v;

						for(uint i = mIdx, n = N ; n ; --n, i = dec(i))
						{
							r += mBuf[i] / (N - n + 1);
						}

						mIdx = inc(mIdx);

						return r / mDamp;
					}

				private:
					static constexpr uint inc(uint v) { return (v + 1) % N; }
					static constexpr uint dec(uint v) { return (v + N - 1) % N; }

					static T calcDamp( )
					{
						T d = 0;

						for(uint i = 0 ; i < N ; ++i)
						{
							d += static_cast<T>(1) / (N - i);
						}

						return d;
					}

				private:
					uint mIdx;
					const T mDamp;
					std::array<T, N> mBuf;
			};
		}
	}
}

#endif
