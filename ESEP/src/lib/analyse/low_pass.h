#ifndef ESEP_ANALYSE_LOWPASS_H
#define ESEP_ANALYSE_LOWPASS_H

#include <array>

#include "lib/utils.h"

namespace esep
{
	namespace analyse
	{
		template<typename T, size_t N>
		class LowPass
		{
			public:
			static constexpr size_t SAMPLES = N;

			public:
				LowPass( ) : mIdx(0) { for(uint i = 0 ; i < N ; ++i) mBuf[i] = 0; }
				T operator()(const T& v)
				{
					T r = 0;

					mBuf[mIdx] = v;

					for(uint i = mIdx, n = N ; n ; --n, i = dec(i))
					{
						r += mBuf[i] / (N - n + 1);
					}

					mIdx = inc(mIdx);

					return r;
				}

			private:
				static constexpr uint inc(uint v) { return (v + 1) % N; }
				static constexpr uint dec(uint v) { return (v + N - 1) % N; }

			private:
				uint mIdx;
				std::array<T, N> mBuf;
		};
	}
}

#endif
