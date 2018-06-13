#ifndef ESEP_LIB_PROCESSOR
#define ESEP_LIB_PROCESSOR

#include <memory>

namespace esep
{
	namespace lib
	{
		template<typename K, typename P>
		class Processor
		{
			typedef K key_type;
			typedef P value_type;
			typedef std::pair<key_type, std::unique_ptr<value_type>> container_type;

			public:
				value_type& use(const key_type& k)
				{
					if(!static_cast<bool>(mProcessor.second) || mProcessor.first != k)
					{
						mProcessor.first = k;
						mProcessor.second.reset(new value_type(k));
					}

					return *mProcessor.second;
				}

				void clear( ) { mProcessor.second.reset(); }

			private:
				container_type mProcessor;
		};
	}
}

#endif
