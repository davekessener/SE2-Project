#ifndef ESEP_LIB_BYTESTREAM_H
#define ESEP_LIB_BYTESTREAM_H

#include <deque>

#include "lib/utils.h"
#include "lib/member_wrapper.h"

namespace esep
{
	namespace lib
	{
		namespace byte_stream
		{
			struct defaults
			{
			    typedef std::deque<byte_t> container_type;

			    typedef MemberWrapper
			    <
			        void,
			        container_type,
			        container_type::const_reference
			    > insert_fn;

			    typedef ConstMemberWrapper
			    <
			        container_type::const_reference,
			        container_type
			    > retrieve_fn;

			    typedef MemberWrapper<void, container_type> remove_fn;
			};
		}

		template<typename C = byte_stream::defaults::container_type>
		class Base_ByteStream
		{
		    public:
		    typedef C container_type;
		    typedef typename C::iterator iterator;
		    typedef typename C::const_iterator const_iterator;

		    public:
		        size_t size( ) const { return mSize; }
		        iterator begin( ) { return mContainer.begin(); }
		        iterator end( ) { return mContainer.end(); }
		        const_iterator begin( ) const { return mContainer.cbegin(); }
		        const_iterator end( ) const { return mContainer.cend(); }
		        const_iterator cbegin( ) const { return mContainer.cbegin(); }
		        const_iterator cend( ) const { return mContainer.cend(); }
		    protected:
		        container_type& getContainer( ) { return mContainer; }
		        void increment( ) { ++mSize; }
		        void decrement( ) { --mSize; }
		    private:
		        container_type mContainer;
		        size_t mSize = 0;
		};

		template
		<
		    typename C = byte_stream::defaults::container_type,
		    typename I = byte_stream::defaults::insert_fn
		>
		class Out_ByteStream : public virtual Base_ByteStream<C>
		{
		    public:
		    typedef typename Base_ByteStream<C>::container_type container_type;
		    typedef I insert_fn;

		    using Base_ByteStream<C>::getContainer;
		    using Base_ByteStream<C>::increment;

		    public:
		        Out_ByteStream(insert_fn i = &container_type::push_back)
		            : mInsert(i) { }
		        void insert(byte_t v)
		        {
		            mInsert(getContainer(), v);
		            increment();
		        }
		    private:
		        insert_fn mInsert;
		};

		template
		<
		    typename C = byte_stream::defaults::container_type,
		    typename G = byte_stream::defaults::retrieve_fn,
		    typename R = byte_stream::defaults::remove_fn
		>
		class In_ByteStream : public virtual Base_ByteStream<C>
		{
		    public:
		    typedef typename Base_ByteStream<C>::container_type container_type;
		    typedef G retrieve_fn;
		    typedef R remove_fn;

		    using Base_ByteStream<C>::getContainer;
		    using Base_ByteStream<C>::decrement;

		    public:
		        In_ByteStream(
		            retrieve_fn get = &container_type::front,
		            remove_fn rem = &container_type::pop_front)
		                : mGet(get), mRemove(rem) { }
		            byte_t remove( )
		            {
		                byte_t v = mGet(getContainer());
		                mRemove(getContainer());
		                decrement();
		                return v;
		            }
		    private:
		        retrieve_fn mGet;
		        remove_fn mRemove;
		};

		template
		<
		    typename C = byte_stream::defaults::container_type,
		    typename I = byte_stream::defaults::insert_fn,
		    typename G = byte_stream::defaults::retrieve_fn,
		    typename R = byte_stream::defaults::remove_fn
		>
		class InOut_ByteStream
		: public In_ByteStream<C, G, R>
		, public Out_ByteStream<C, I>
		{
		    public:
		    typedef typename Base_ByteStream<C>::container_type container_type;
		    typedef typename Out_ByteStream<C, I>::insert_fn insert_fn;
		    typedef typename In_ByteStream<C, G, R>::retrieve_fn retrieve_fn;
		    typedef typename In_ByteStream<C, G, R>::remove_fn remove_fn;

		    public:
		        InOut_ByteStream(
		            insert_fn in = &container_type::push_back,
		            retrieve_fn get = &container_type::front,
		            remove_fn rem = &container_type::pop_front)
		                : In_ByteStream<C, G, R>(get, rem)
		                , Out_ByteStream<C, I>(in) { }
		    private:
		};

		typedef InOut_ByteStream<> ByteStream;
	}
}

template<typename T, typename C, typename I>
esep::lib::Out_ByteStream<C, I>& operator<<(esep::lib::Out_ByteStream<C, I>& os, T&& o)
{
    const byte_t *p = reinterpret_cast<const byte_t *>(&o);

    for(size_t n = sizeof(T) ; n ; --n)
    {
        os.insert(*p++);
    }

    return os;
}

template<typename T, typename C, typename G, typename R>
esep::lib::In_ByteStream<C, G, R>& operator>>(esep::lib::In_ByteStream<C, G, R>& is, T&& o)
{
    byte_t *p = reinterpret_cast<byte_t *>(&o);

    for(size_t n = sizeof(T) ; n ; --n)
    {
        *p++ = is.remove();
    }

    return is;
}

#endif
