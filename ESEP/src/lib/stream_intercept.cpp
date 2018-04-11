#include "lib/stream_intercept.h"

namespace esep { namespace lib {

StreamIntercept::~StreamIntercept(void)
{
	mRestore(mSink);
}

std::streambuf::int_type StreamIntercept::overflow(int_type m)
{
	if(traits_type::eq_int_type(m, traits_type::eof()))
		return mTarget->pubsync() == -1 ? m: traits_type::not_eof(m);

	return mTarget->sputc(m);
}

}}
