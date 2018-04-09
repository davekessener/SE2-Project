#ifndef ESEP_LIB_BYTEBUFFER_H
#define ESEP_LIB_BYTEBUFFER_H

#include "lib/member_wrapper.h"

namespace esep
{
	namespace lib
	{
		template
		<
			typename C = std::deque<byte_t>
		>
		class Base_ByteStream
		{
			public:
			protected:
				C& container( ) { return mContainer; }
			private:
				C mContainer;
		};


		template
		<
			typename C = std::vector<byte_t>,
			typename Insert = MemberWrapper<void, C, const T&>
		>
		class ByteBuffer
		{
			typedef C container_type;
			typedef Insert insert_fn;

			public:
				ByteBuffer(insert_fn f = &container_type::push_back)
					: mInsert(f) { }
				iterator begin( ) { return mContainer.begin(); }
				iterator end( ) { return mContainer.end(); }
				const_iterator begin( ) const { return mContainer.cbegin(); }
				const_iterator end( ) const { return mContainer.cend(); }
				const_iterator cbegin( ) const { return mContainer.cbegin(); }
				const_iterator cend( ) const { return mContainer.cend(); }
				size_t size( ) const { return mContainer.size(); }
				byte_t checksum( ) const
					{ byte_t c = 0; for(const byte_t& e : mContainer) c ^= e; return c; }
				template<typename T>
					void insert(const T&);
				template<typename T, size_t N>
					void insert(const T (&a)[N])
						{ for(const T *i1 = a, *i2 = a + N ; i1 != i2 ; ++i1) insert(*i1); }
				template<typename C2, typename I2>
					void insert(const ByteBuffer<C2, I2>& b)
						{ for(const byte_t& e : b) insert(e); }
			private:
				container_type mContainer;
				insert_fn mInsert;
		};

		template<typename C, typename I>
		template<typename T>
		void ByteBuffer<C, I>::insert(T&& o)
		{
			const byte_t *p = reinterpret_cast<const int8_t *>(&o);

			for(uint i = 0 ; i < sizeof(T) ; ++i)
			{
				mInsert(mContainer, *p++);
			}
		}
	}
}

template<typename C, typename I, typename T>
ByteBuffer<C, I>& operator<<(ByteBuffer<C, I>& bb, T&& o)
{
	bb.template insert<T>(o);

	return bb;
}

#endif
