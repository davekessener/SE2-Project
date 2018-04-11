#ifndef ESEP_LIB_RINGBUFFER_H
#define ESEP_LIB_RINGBUFFER_H

#include <stdexcept>
#include <array>

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		struct BufferSizeException : public std::runtime_error
		{
			BufferSizeException(const std::string& s) : std::runtime_error(s) { }
		};

		template<typename T, size_t N>
		class RingBuffer
		{
			public:
				RingBuffer( );
				void insert(const T&);
				T& top( );
				void remove( );
				size_t size( ) const { return mSize; }
			private:
				std::array<T, N> mBuffer;
				uint mRead, mWrite, mSize;
		};

		template<typename T, size_t N>
		RingBuffer<T, N>::RingBuffer(void)
		{
			mRead = mWrite = mSize = 0;
		}

		template<typename T, size_t N>
		void RingBuffer<T, N>::insert(const T& o)
		{
			if(mSize == N)
				MXT_THROW_E(BufferSizeException, "Can't insert, the buffer is full!");

			mBuffer[mWrite++] = o;
			mWrite %= N;
			++mSize;
		}

		template<typename T, size_t N>
		T& RingBuffer<T, N>::top(void)
		{
			if(mSize == 0)
				MXT_THROW_E(BufferSizeException, "Can't get, the buffer is empty!");

			return mBuffer[mRead];
		}

		template<typename T, size_t N>
		void RingBuffer<T, N>::remove(void)
		{
			if(mSize == 0)
				MXT_THROW_E(BufferSizeExcpetion, "Can't remove, the buffer is empty!");

			mRead = (mRead + 1) % N;
			--mSize;
		}
	}
}

#endif
