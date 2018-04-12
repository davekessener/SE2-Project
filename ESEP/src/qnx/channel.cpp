#include <sys/neutrino.h>

#include "qnx/channel.h"

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

}}
