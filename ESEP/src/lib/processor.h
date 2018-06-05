#ifndef ESEP_LIB_PROCESSOR_H
#define ESEP_LIB_PROCESSOR_H

#include "lib/overflow_buffer.h"

namespace esep
{
	namespace lib
	{
		template
		<
			typename T,
			typename P,
			size_t N = 10
		>
		class Processor
		{
			typedef T key_type;
			typedef P value_type;
			typedef std::unique_ptr<value_type> handle_t;
			typedef std::pair<key_type, handle_t> entry_t;
			typedef OverflowBuffer<entry_t, N> container_type;

			public:
				value_type& processor(const key_type& k)
				{
					for(auto& e : mBuf)
					{
						if(e.first == k)
						{
							return *e.second;
						}
					}

					value_type *v = new value_type(k);

					mBuf.insert(std::make_pair(k, handle_t(v)));

					return *v;
				}

			private:
				container_type mBuf;
		};
	}
}

#endif
