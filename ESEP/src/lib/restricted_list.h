#ifndef ESEP_LIB_RESTRICTED_MAP_H
#define ESEP_LIB_RESTRICTED_MAP_H

#include <array>

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		template<typename T, size_t N>
		class RestrictedList
		{
			public:
			typedef T value_type;
			typedef std::array<value_type, N> container_type;
			static constexpr size_t Size = N;
			typedef T *iterator;
			typedef const T *const_iterator;

			public:
			iterator begin( ) { return &mContent[0]; }
			iterator end( ) { return begin() + size(); }
			const_iterator begin( ) const { return &mContent[0]; }
			const_iterator end( ) const { return begin() + size(); }
			const_iterator cbegin( ) const { return &mContent[0]; }
			const_iterator cend( ) const { return begin() + size(); }
			size_t size( ) const { return mFull ? Size : mIdx; }
			void insert(const value_type& e) { mContent[mIdx = (mIdx + 1) % N] = e; if(!mIdx) mFull = true; }
			void clear( ) { mFull = false; mIdx = 0; }
			private:
				container_type mContent;
				uint mIdx = 0;
				bool mFull = false;
		};
	}
}

#endif
