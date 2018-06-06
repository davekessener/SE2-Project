#ifndef ESEP_ANALYSE_TRIM_H
#define ESEP_ANALYSE_TRIM_H

#include "lib/stream/stream.h"

namespace esep
{
	namespace analyse
	{
		template<typename T>
		class Trim
		{
			typedef Trim<T> Self;
			typedef lib::stream::Stream<T> stream_t;
			typedef std::function<bool(const T&)> pred_fn;

			public:
				Trim( ) : mSize(0) { }
				Trim(stream_t&& s, pred_fn f, size_t n)
					: mStream(std::move(s))
					, mPred(std::move(f))
					, mSize(n)
				{
					while(n--)
					{
						T o(mStream.remove());

						if(!mPred(o))
						{
							mBuf.emplace_back(std::move(o));
							break;
						}
					}

					refill();
				}

				T& operator*( ) { return mBuf.front(); }
				Self& operator++( ) { mBuf.pop_front(); refill(); return *this; }
				Self operator++(int) { Self o(*this); ++*this; return *this; }

				bool operator==(const Self& s) const { return s.mBuf.empty() && mBuf.empty(); }
				bool operator!=(const Self& s) const { return !(*this == s); }

			private:
				void refill(void)
				{
					if(!mStream.empty())
					{
						while(mBuf.size() < mSize)
						{
							if(mStream.empty()) break;

							mBuf.emplace_back(mStream.remove());
						}

						if(mStream.empty())
						{
							for(size_t n = mSize > mBuf.size() ? mBuf.size() : mSize ; n > 0 ; --n)
							{
								if(mPred(mBuf[n - 1])) mBuf.pop_back();
							}
						}
					}
				}

			private:
				stream_t mStream;
				pred_fn mPred;
				const size_t mSize;
				std::deque<T> mBuf;
		};
	}
}

#endif
