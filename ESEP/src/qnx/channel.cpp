#include <sys/neutrino.h>

#include "qnx/channel.h"

#define MXT_HWINT 97

namespace esep { namespace qnx {

Channel::Channel(uint32_t f)
	: mID(0)
{
	if((mID = ChannelCreate(f)) == -1)
	{
		MXT_THROW("Failed to open channel!");
	}
}

Channel::~Channel(void)
{
	ChannelDestroy(mID);
}

Connection Channel::connect(void)
{
	return Connection(mID);
}

pulse_t Channel::receivePulse(void)
{
	pulse_t r;
	struct _pulse pulse;

	if(MsgReceivePulse(mID, &pulse, sizeof(pulse), nullptr) == -1)
	{
		MXT_THROW("Error while receiving pulse message on connection ", mID, "!");
	}

	r.code = pulse.code;
	r.value = pulse.value.sival_int;

	return r;
}

void Channel::listenForInterrupts(Connection& c)
{
	ThreadCtl(_NTO_TCTL_IO, 0);

	struct sigevent e;

	SIGEV_PULSE_INIT(&e, c.mID, SIGEV_PULSE_PRIO_INHERIT, static_cast<int8_t>(Code::INTERRUPT), 0);

	if((mIntID = InterruptAttachEvent(MXT_HWINT, &e, 0)) == -1)
	{
		MXT_THROW("Failed to attach interrupt event!");
	}
}

}}
