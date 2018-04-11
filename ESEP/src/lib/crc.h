#ifndef ESEP_LIB_CRC_H
#define ESEP_LIB_CRC_H

#include <stdint.h>

namespace esep
{
	namespace lib
	{
		namespace crc
		{
			constexpr uint32_t DEFAULT_POLYNOMIAL = 0xEDB88320;

			template<typename T = uint32_t>
			class Generator
			{
				static constexpr uint N = 0x100;

				public:
				typedef T crc_t;

				public:
					Generator(T = DEFAULT_POLYNOMIAL);
					template<typename I>
						crc_t encode(I&&, I&&);
					template<typename TT, size_t M>
						crc_t encode(const TT (&a)[M])
							{ return encode(&*a, &*a + M); }
					template<typename TT>
						crc_t encode(TT&& a)
							{ return encode(std::begin(a), std::end(a)); }
				private:
					T mPolynomial;
					T mTable[N];
			};

			template<typename T>
			Generator<T>::Generator(T p)
				: mPolynomial(p)
			{
				for(uint i = 0 ; i < N ; ++i)
				{
					T c = i;

					for(uint j = 0 ; j < 8 ; ++j)
					{
						c = ((c & 1) ? mPolynomial : 0) ^ (c >> 1);
					}

					mTable[i] = c;
				}
			}

			template<typename T>
			template<typename I>
			typename Generator<T>::crc_t Generator<T>::encode(I&& i1, I&& i2)
			{
				crc_t crc = static_cast<crc_t>(-1);

				for(; i1 != i2 ; ++i1)
				{
					crc = mTable[(crc ^ *i1) % N] ^ (crc >> 8);
				}

				return ~crc;
			}
		}
	}
}

#endif
