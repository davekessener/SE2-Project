#ifndef ESEP_LIB_BYTESTREAM_H
#define ESEP_LIB_BYTESTREAM_H

#include <deque>

#include "lib/utils.h"
#include "lib/member_wrapper.h"
#include "lib/tml.h"

namespace esep
{
	namespace lib
	{
		namespace byte_stream
		{
			struct defaults
			{
			    typedef std::deque<byte_t> container_type;
			};

			template<typename I>
			struct IterableWrapper
			{
				IterableWrapper(I i1, I i2) : i1(i1), i2(i2) { }
				I i1, i2;
			};

			template<typename T>
			auto insert_all(T&& o) -> IterableWrapper<decltype(std::begin(o))>
			{
				typedef decltype(std::begin(o)) iterator;

				return IterableWrapper<iterator>(std::begin(o), std::end(o));
			}

			template<typename I1, typename I2>
			IterableWrapper<tml::DoDecay<I1>> insert_all(I1&& i1, I2&& i2)
			{
				return IterableWrapper<tml::DoDecay<I1>>(i1, i2);
			}
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
		        bool empty( ) const { return mSize == 0; }
		        void clear( ) { Base_ByteStream<C> bs; swap(bs); }
		        iterator begin( ) { return mContainer.begin(); }
		        iterator end( ) { return mContainer.end(); }
		        const_iterator begin( ) const { return mContainer.cbegin(); }
		        const_iterator end( ) const { return mContainer.cend(); }
		        const_iterator cbegin( ) const { return mContainer.cbegin(); }
		        const_iterator cend( ) const { return mContainer.cend(); }
		        void swap(Base_ByteStream<C>& s) { std::swap(mContainer, s.mContainer); std::swap(mSize, s.mSize); }
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
		    typename C = byte_stream::defaults::container_type
		>
		class Out_ByteStream : public virtual Base_ByteStream<C>
		{
		    public:
		    typedef typename Base_ByteStream<C>::container_type container_type;
		    typedef std::function<void(container_type&, byte_t)> insert_fn;

		    using Base_ByteStream<C>::getContainer;
		    using Base_ByteStream<C>::increment;

		    public:
		        Out_ByteStream(insert_fn i = [](container_type& c, byte_t v) { c.push_back(v); })
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
		    typename C = byte_stream::defaults::container_type
		>
		class In_ByteStream : public virtual Base_ByteStream<C>
		{
		    public:
		    typedef typename Base_ByteStream<C>::container_type container_type;
		    typedef std::function<byte_t(container_type&)> remove_fn;

		    using Base_ByteStream<C>::getContainer;
		    using Base_ByteStream<C>::decrement;

		    public:
		        In_ByteStream(
		            remove_fn rem = [](container_type& c) { byte_t v(c.front()); c.pop_front(); return v; })
		                : mRemove(rem) { }
		            byte_t remove( )
		            {
		            	decrement();
		                return mRemove(getContainer());
		            }
		    private:
		        remove_fn mRemove;
		};

		template
		<
		    typename C = byte_stream::defaults::container_type
		>
		class InOut_ByteStream
		: public In_ByteStream<C>
		, public Out_ByteStream<C>
		{
		    public:
		    typedef typename Base_ByteStream<C>::container_type container_type;
		    typedef typename Out_ByteStream<C>::insert_fn insert_fn;
		    typedef typename In_ByteStream<C>::remove_fn remove_fn;

		    using Base_ByteStream<C>::cbegin;
		    using Base_ByteStream<C>::size;

		    public:
		        InOut_ByteStream(
		            insert_fn in = [](container_type& c, byte_t v) { c.push_back(v); },
		            remove_fn rem = [](container_type& c) { byte_t v(c.front()); c.pop_front(); return v; })
		                : In_ByteStream<C>(rem)
		                , Out_ByteStream<C>(in) { }
		        InOut_ByteStream<C> section(size_t p1, size_t p2) const
				{
		        	if(p1 > p2 || p2 > size())
		        		MXT_THROW("Invalid section boundaries (buffer size is ", size(), "): [", p1, ", ", p2, ")!");

		        	InOut_ByteStream<C> r;
		        	auto i = cbegin();

		        	while(p1) { ++i; --p1; --p2; }
		        	while(p2) { r.insert(*i++); --p2; }

		        	return r;
				}
		    private:
		};

		typedef InOut_ByteStream<> ByteStream;

		namespace byte_stream
		{
			template<typename C>
			ByteStream from_container(C&& c)
			{
				ByteStream bs;

				bs << insert_all(c);

				return bs;
			}
		}
	}
}

template<typename T, typename C>
esep::lib::Out_ByteStream<C>& operator<<(esep::lib::Out_ByteStream<C>& os, T&& o)
{
    const byte_t *p = reinterpret_cast<const byte_t *>(&o);

    for(size_t n = sizeof(T) ; n ; --n)
    {
        os.insert(*p++);
    }

    return os;
}

template<typename T, typename C>
esep::lib::Out_ByteStream<C>& operator<<(esep::lib::Out_ByteStream<C>& os, esep::lib::byte_stream::IterableWrapper<T> w)
{
	auto i = w.i1;

	while(i != w.i2)
	{
		os.insert(*i++);
	}

	return os;
}

template<typename T, typename C>
esep::lib::In_ByteStream<C>& operator>>(esep::lib::In_ByteStream<C>& is, T&& o)
{
    byte_t *p = reinterpret_cast<byte_t *>(&o);

    for(size_t n = sizeof(T) ; n ; --n)
    {
        *p++ = is.remove();
    }

    return is;
}

#endif
