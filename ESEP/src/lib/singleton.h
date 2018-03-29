#ifndef ESEP_LIB_SINGLETON_H
#define ESEP_LIB_SINGLETON_H

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		template<typename T>
		class SingletonHolder
		{
			public:
				static T& instance( )
					{ static T inst; return inst; }
			private:
				SingletonHolder( ) = delete;
				~SingletonHolder( ) = delete;
				SingletonHolder(const SingletonHolder<T>&) = delete;
		};
	}
}

#endif

