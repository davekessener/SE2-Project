#ifndef ESEP_LIB_STREAM_H
#define ESEP_LIB_STREAM_H

#include "lib/stream/iterator.h"
#include "lib/stream/container_iter.h"
#include "lib/stream/adapter_iter.h"
#include "lib/stream/buffer_iter.h"

#include "lib/utils.h"
#include "lib/tml.h"
#include "lib/function.h"

namespace esep
{
	namespace lib
	{
		namespace stream
		{
			template<typename T>
			class Stream
			{
				typedef Stream<T> Self;
				typedef BufferIterator<T> buffer_t;

				template<typename TT>
				using Iterator_ptr = typename Iterator<TT>::Iterator_ptr;

				public:
					Stream( ) : mBuf(nullptr) { }

					template<typename I>
						Stream(I&& i1, I&& i2) : mI(new ContainerIterator<T, tml::DoDecay<I>>(std::forward<I>(i1), std::forward<I>(i2))), mBuf(nullptr) { }

					template<typename C>
						explicit Stream(const C& c)
							: mI(new ContainerIterator<T, typename C::const_iterator>(std::begin(c), std::end(c)))
							, mBuf(nullptr) { }

					template<size_t N>
						explicit Stream(const T (&a)[N]) : mI(new ContainerIterator<T, T *>(a, a + N)), mBuf(nullptr) { }

					Stream(const Self& s) : mBuf(nullptr)
					{
						if(!s.empty())
						{
							mI = s.mI->clone();

							if(s.mBuf)
							{
								mBuf = dynamic_cast<buffer_t *>(mI.get());
							}
						}
					}

					Stream(Self&& s) : mI(std::move(s.mI)), mBuf(s.mBuf) { s.mBuf = nullptr; }

					Self& operator=(Self s)
					{
						s.swap(*this);

						return *this;
					}

					void swap(Self& s) noexcept
					{
						std::swap(mI, s.mI);
						std::swap(mBuf, s.mBuf);
					}

					bool empty( ) const { return !mI.get() || !mI->hasNext(); }
					T remove( ) { return mI->next(); }

					template
					<
						typename F,
						typename R = typename FunctionTraits<F>::return_type,
						typename = tml::EnableIf<
							tml::Equals<
								tml::DoDecay<
									tml::list::DoGet<
										typename FunctionTraits<F>::argument_types,
										0>>,
								T>>
					>
					Stream<R> map(F&& f) { return Stream<R>(Iterator_ptr<R>(new AdapterIterator<R, T>(mI->clone(), std::forward<F>(f)))); }

					template<typename F>
					F collect(F&& f)
					{
						while(mI->hasNext())
						{
							f(mI->next());
						}

						return std::forward<F>(f);
					}

					template<typename F>
					T reduce(F&& f, T a = T())
					{
						while(mI->hasNext())
						{
							a = f(a, mI->next());
						}

						return std::move(a);
					}

					template<typename F>
					T select(F&& f)
					{
						T v = mI->next();

						while(mI->hasNext())
						{
							T t = mI->next();

							if(f(v, t))
							{
								v = t;
							}
						}

						return v;
					}

					template<typename TT>
					void emplace_front(TT&& o)
					{
						getBuffer()->emplace_front(std::forward<TT>(o));
					}

					template<typename TT>
					void emplace_back(TT&& o)
					{
						getBuffer()->emplace_back(std::forward<TT>(o));
					}

					void push_front(const T& o) { emplace_front(T(o)); }
					void push_back(const T& o)  { emplace_back(T(o));  }

				private:
					Stream(Iterator_ptr<T> i) : mI(i), mBuf(nullptr) { }

					buffer_t *getBuffer( )
					{
						if(!mBuf)
						{
							mI.reset(mBuf = new buffer_t(mI));
						}

						return mBuf;
					}

				private:
					Iterator_ptr<T> mI;
					buffer_t *mBuf;

					template<typename TT>
					friend class Stream;
			};
		}
	}
}

#endif
