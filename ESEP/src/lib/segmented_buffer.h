#ifndef ESEP_LIB_SEGMENTEDBUFFER_H
#define ESEP_LIB_SEGMENTEDBUFFER_H

#include <deque>

#include "lib/utils.h"

#include "lib/logger.h"

namespace esep
{
	namespace lib
	{
		template
		<
			typename T,
			size_t N,
			typename C = std::deque<T>
		>
		class SegmentedBuffer
		{
			public:
			typedef T value_type;
			typedef C container_type;
			typedef typename container_type::iterator iterator;
			typedef int segment_t;

			MXT_DEFINE_E(OutOfBoundsException);
			MXT_DEFINE_E(EmptySegmentException);

			static constexpr segment_t NO_SEGMENTS = N;

			public:
				SegmentedBuffer(uint);
				void advance(segment_t);
				void remove(segment_t);
				value_type& get(segment_t);
				iterator begin(segment_t = 0);
				iterator end(segment_t = NO_SEGMENTS);

			private:
				void do_remove(segment_t s) { --mSizes[s]; }

			private:
				const uint mBuffer;
				container_type mContainer;
				size_t mSizes[NO_SEGMENTS + 1];
		};

		template<typename T, size_t N, typename C>
		SegmentedBuffer<T, N, C>::SegmentedBuffer(uint b) : mBuffer(b)
		{
			for(uint i = 0 ; i < NO_SEGMENTS + 1 ; ++i)
			{
				mSizes[i] = 0;
			}
		}

		template<typename T, size_t N, typename C>
		void SegmentedBuffer<T, N, C>::advance(segment_t s)
		{
			if(s < 0 || s > NO_SEGMENTS)
			{
				MXT_THROW_EX(OutOfBoundsException);
			}

			if(!s)
			{
				mContainer.emplace_front();
			}
			else
			{
				do_remove(s - 1);
			}

			++mSizes[s];

			while(mSizes[NO_SEGMENTS] > mBuffer)
			{
				--mSizes[NO_SEGMENTS];
				mContainer.pop_back();
			}
		}

		template<typename T, size_t N, typename C>
		void SegmentedBuffer<T, N, C>::remove(segment_t s)
		{
			if(s < 0 || s > NO_SEGMENTS)
			{
				MXT_THROW_EX(OutOfBoundsException);
			}

			if(!mSizes[s])
			{
				MXT_THROW_EX(EmptySegmentException);
			}

			mContainer.erase(--end(s));
			do_remove(s);
		}

		template<typename T, size_t N, typename C>
		typename SegmentedBuffer<T, N, C>::value_type& SegmentedBuffer<T, N, C>::get(segment_t s)
		{
			if(s < 0 || s > NO_SEGMENTS)
			{
				MXT_THROW_EX(OutOfBoundsException);
			}

			if(!mSizes[s])
			{
				MXT_THROW_EX(EmptySegmentException);
			}

			return *--end(s);
		}

		template<typename T, size_t N, typename C>
		typename SegmentedBuffer<T, N, C>::iterator SegmentedBuffer<T, N, C>::begin(segment_t s)
		{
			if(s < 0 || s > NO_SEGMENTS)
			{
				MXT_THROW_EX(OutOfBoundsException);
			}

			auto i = std::begin(mContainer);

			for(uint j = 0 ; j < (uint) s ; ++j)
			{
				for(uint k = 0 ; k < mSizes[j] ; ++k)
				{
					++i;
				}
			}

			return i;
		}

		template<typename T, size_t N, typename C>
		typename SegmentedBuffer<T, N, C>::iterator SegmentedBuffer<T, N, C>::end(segment_t s)
		{
			auto i = begin(s);

			for(uint j = 0 ; j < mSizes[s] ; ++j) ++i;

			return i;
		}
	}
}

#endif
