#ifndef ESEP_ANALYSE_EXPAND_H
#define ESEP_ANALYSE_EXPAND_H

#include "lib/stream/stream.h"

namespace esep
{
	namespace analyse
	{
		template<typename T>
		class Expand
		{
			typedef Expand<T> Self;
			typedef typename T::first_type key_type;
			typedef typename T::second_type value_type;
			typedef lib::stream::Stream<T> stream_t;

			enum class State { RUN, EMPTY, DONE };

			public:
				Expand( ) : mState(State::DONE) { }
				Expand(const stream_t& s) : Expand(stream_t(s)) { }
				Expand(stream_t&& s) : mStream(std::move(s)), mState(State::RUN)
				{
					mCur = mStream.remove();
					mNext = mStream.remove();
				}

				T operator*( ) { return mCur; }

				Self& operator++( )
				{
					switch(mState)
					{
					case State::RUN:
						if(++mCur.first >= mNext.first)
						{
							mCur = mNext;

							if(!mStream.empty())
							{
								mNext = mStream.remove();
							}
							else
							{
								mState = State::EMPTY;
							}
						}
						break;

					case State::EMPTY:
						mState = State::DONE;
						break;

					case State::DONE:
						break;
					}

					return *this;
				}

				Self operator++(int) { Self e(*this); ++*this; return e; }

				bool operator==(const Self& e) const { return e.mState == State::DONE && mState == State::DONE; }
				bool operator!=(const Self& e) const { return !(*this == e); }

			private:
				stream_t mStream;
				T mCur, mNext;
				State mState;
		};
	}
}

#endif
