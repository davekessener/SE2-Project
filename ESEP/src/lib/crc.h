#ifndef ESEP_LIB_CRC_H
#ifndef ESEP_LIB_CRC_H

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
					Generator(T);
					template<typename I>
						crc_t encode(I&&, I&&);
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
			Generator<T>::crc_t Generator<T>::encode(I&& i1, I&& i2)
			{
				crc_t crc = static_cast<crc_t>(-1);

				for(; i1 != i2 ; ++i1)
				{
					c = mTable[(crc ^ *i1) % N] ^ (c >> 8);
				}

				return ~crc;
			}
		}
	}
}

#endif
