#ifndef ESEP_LIB_ALGORITHMS_H
#define ESEP_LIB_ALGORITHMS_H

namespace esep
{
	namespace lib
	{
		/**
		 * Calculates simple XOR checksum of
		 * passed iterable object of type T
		 */
		template<typename T>
		byte_t checksum(T&& o)
		{
			byte_t v = 0;

			for(const byte_t& e : o)
			{
				v ^= e;
			}

			return v;
		}
	}
}

#endif
