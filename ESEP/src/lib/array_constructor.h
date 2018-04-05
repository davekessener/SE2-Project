#ifndef ESEP_LIB_ARRAYCONSTRUCTOR_H
#define ESEP_LIB_ARRAYCONSTRUCTOR_H

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		template<typename T>
		class ArrayConstructor
		{
			public:
				template<size_t N>
					explicit ArrayConstructor(T *(&)[N]);
				~ArrayConstructor( );
				void add(T *);
				void finalize( );
				uint size( ) const { return mSize; }
			private:
				uint mSize, mCapacity;
				T **mArray;
		};

		template<typename T>
		template<size_t N>
		ArrayConstructor<T>::ArrayConstructor(T *(&a)[N])
			: mSize(0)
			, mCapacity(N)
			, mArray(a)
		{
			for(auto& p : a)
			{
				p = nullptr;
			}
		}

		template<typename T>
		ArrayConstructor<T>::~ArrayConstructor(void)
		{
			if(mArray) for(uint i = 0 ; i < mCapacity ; ++i)
			{
				delete mArray[i];
			}
		}

		template<typename T>
		void ArrayConstructor<T>::add(T *p)
		{
			if(!mArray)
			{
				throw std::string("ArrayConstructor already taken!");
			}

			if(mSize == mCapacity)
			{
				throw lib::stringify("Exceeding max capacity of ", mCapacity);
			}

			mArray[mSize++] = p;
		}

		template<typename T>
		void ArrayConstructor<T>::finalize(void)
		{
			mArray = nullptr;
		}
	}
}

#endif
