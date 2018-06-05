#ifndef ESEP_LIB_OVERFLOWBUFFER_H
#define ESEP_LIB_OVERFLOWBUFFER_H

#include <array>

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		template
		<
			typename T,
			size_t N
		>
		class OverflowBuffer
		{
			typedef std::array<T, N> container_type;

			template<typename TT>
			class itor
			{
				public:
				typedef TT value_type;
				typedef tml::DoIf<tml::IsConst<TT>,
				tml::Type2Type<const std::array<tml::DoDecay<TT>, N>>,
					tml::Type2Type<std::array<TT, N>>>
				container_type;

				public:
					itor( ) : mBuf(nullptr), mTraversed(0), mOffset(0), mSize(nullptr) { }

					value_type& operator*( ) { return *get(); }
					itor<TT> operator++( ) { if(mBuf && ++mTraversed == *mSize) { mBuf = nullptr; } return *this; }
					itor<TT> operator++(int) { itor<TT> i(*this); ++*this; return i; }

					bool operator==(const itor<TT>& i) const { return (!mBuf && !i.mBuf) || (get() == i.get()); }
					bool operator!=(const itor<TT>& i) const { return !(*this == i); }

				private:
					itor(container_type *c, const uint *s, uint o) : mBuf(c), mTraversed(0), mOffset(o), mSize(s) { }

					value_type *get( ) { return mBuf ? &(*mBuf)[0] + ((mTraversed + mOffset) % *mSize) : nullptr; }
					const value_type *get( ) const { return mBuf ? &(*mBuf)[0] + ((mTraversed + mOffset) % *mSize) : nullptr; }

				private:
					container_type *mBuf;
					uint mTraversed, mOffset;
					const uint *mSize;

					friend class OverflowBuffer<tml::DoDecay<TT>, N>;
			};

			public:
			typedef T value_type;
			typedef itor<value_type> iterator;
			typedef itor<const value_type> const_iterator;

			public:
				OverflowBuffer( ) : mSize(0), mIdx(0) { }
				void insert(const value_type& e) { insert(value_type(e)); }
				void insert(value_type&& e)
				{
					if(mSize < N)
					{
						mBuf[mSize++] = std::move(e);
					}
					else
					{
						mBuf[mIdx] = std::move(e); mIdx = (mIdx + 1) % N;
					}
				}

				size_t size( ) const { return mSize; }
				bool empty( ) const { return !mSize; }
				bool full( ) const { return mSize == N; }
				static constexpr size_t capacity( ) { return N; }

				iterator begin( ) { return empty() ? end() : iterator(&mBuf, &mSize, mIdx); }
				iterator end( ) { return iterator(); }
				const_iterator begin( ) const { return empty() ? end() : const_iterator(&mBuf, &mSize, mIdx); }
				const_iterator end( ) const { return const_iterator(); }
				const_iterator cbegin( ) const { return begin(); }
				const_iterator cend( ) const { return end(); }

			private:
				container_type mBuf;
				uint mSize, mIdx;
		};
	}
}

#endif
