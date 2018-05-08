#include "emp/recorder.h"

#include "lib/timer.h"
#include "lib/logger.h"

namespace esep { namespace emp {

Recorder::Recorder(Writer_ptr p)
	: mWriter(std::move(p))
{
	Physical::setCallback([this](Event e) {
		record(lib::Timer::instance().elapsed(), e);

		if(static_cast<bool>(mCallback))
		{
			mCallback(e);
		}
	});
}

void Recorder::record(uint64_t t, Event e)
{
}

}}
