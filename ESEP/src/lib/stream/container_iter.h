#ifndef ESEP_LIB_STREAM_CONTAINERITER_H
#define ESEP_LIB_STREAM_CONTAINERITER_H

#include "lib/stream/iterator.h"

namespace esep
{
	namespace lib
	{
		namespace stream
		{
			template<typename T, typename I>
			class ContainerIterator : public Iterator<T>
			{
				typedef ContainerIterator<T, I> Self;
				typedef typename Iterator<T>::Iterator_ptr Iterator_ptr;

				public:
					template<typename I1, typename I2>
					ContainerIterator(I1&& i1, I2&& i2) : mI1(std::forward<I1>(i1)), mI2(std::forward<I2>(i2)) { }

					T next( ) override { return *mI1++; }
					bool hasNext( ) const override { return mI1 != mI2; }

					Iterator_ptr clone( ) const override { return Iterator_ptr(new Self(*this)); }

				private:
					I mI1;
					const I mI2;
			};
		}
	}
}

#endif
