#ifndef ESEP_LIB_STREAM_ITERATOR_H
#define ESEP_LIB_STREAM_ITERATOR_H

#include <memory>

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		namespace stream
		{
			MXT_DEFINE_E(StreamUnderflowException);

			template<typename T>
			struct Iterator
			{
				typedef Iterator<T> Self;
				typedef std::shared_ptr<Self> Iterator_ptr;

				virtual ~Iterator( ) { }

				virtual T next( ) = 0;
				virtual bool hasNext( ) const = 0;

				virtual Iterator_ptr clone( ) const = 0;
			};
		}
	}
}

#endif
