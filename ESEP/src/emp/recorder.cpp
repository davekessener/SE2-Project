#include <sstream>

#include "emp/recorder.h"

#include "emp/location.h"

#include "lib/timer.h"
#include "lib/logger.h"

namespace esep { namespace emp {

Recorder::Recorder(Writer_ptr p, hal::HAL_ptr hal)
	: mHAL(std::move(hal))
	, mWriter(std::move(p))
{
	mHAL->setCallback([this](Event e) {
		uint64_t t = lib::Timer::instance().elapsed();

		if(t >= (1ull << 32))
		{
			MXT_LOG("Can't record events that far along!", log::Severity::WARNING);
		}

		record(t, e);

		if(static_cast<bool>(mCallback))
		{
			mCallback(e);
		}
	});
}

void Recorder::record(uint32_t t, Event e)
{
	std::stringstream ss;
	auto l = Location::byEvent(e);
	auto v = mHAL->in(hal::HAL::getField(e)) & hal::HAL::getPin(e);

	ss << std::setfill('0') << std::setw(8) << t << " " << std::setfill(' ') << std::setw(20) << l.name << " ";

	if(hal::HAL::getField(e) == hal::HAL::Field::ANALOG)
	{
		ss << "$" << lib::hex<16>(v);
	}
	else
	{
		ss << std::boolalpha << static_cast<bool>(v);
	}

	mWriter->writeLine(ss.str());
}

}}
