#ifndef ESEP_LIB_STREAM_BUFFERITER_H
#define ESEP_LIB_STREAM_BUFFERITER_H

#include <deque>

#include "lib/stream/iterator.h"

namespace esep
{
	namespace lib
	{
		namespace stream
		{
			template<typename T, typename C = std::deque<T>>
			class BufferIterator : public Iterator<T>
			{
				typedef BufferIterator<T, C> Self;
				typedef C container_type;
 				typedef typename Iterator<T>::Iterator_ptr Iterator_ptr;

				public:
					BufferIterator(Iterator_ptr i) : mI(i) { }
					BufferIterator(const Self& s) : mI(s.mI->clone()), mFront(s.mFront), mBack(s.mBack) { }

					bool hasNext( ) const override { return !mFront.empty() || mI->hasNext() || !mBack.empty(); }

					T next( ) override
					{
						if(!mFront.empty())
						{
							T r(mFront.front());
							mFront.pop_front();
							return r;
						}

						if(mI->hasNext())
						{
							return mI->next();
						}

						if(!mBack.empty())
						{
							T r(mBack.front());
							mBack.pop_front();
							return r;
						}

						MXT_THROW_EX(StreamUnderflowException);
					}

					Iterator_ptr clone( ) const override { return Iterator_ptr(new Self(*this)); }

					template<typename TT>
					void emplace_front(TT&& o) { mFront.emplace_front(std::forward<TT>(o)); }

					template<typename TT>
					void emplace_back(TT&& o) { mBack.emplace_back(std::forward<TT>(o)); }

				private:
					Iterator_ptr mI;
					container_type mFront, mBack;
			};
		}
	}
}

#endif
