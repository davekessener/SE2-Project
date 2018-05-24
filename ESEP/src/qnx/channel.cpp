#include <sys/neutrino.h>

#include "qnx/channel.h"

#define MXT_HWINT 97

#define MXT_NS_IN_S 1000000000l

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

	if(r.code == mIntCode)
	{
		InterruptUnmask(MXT_HWINT, -1);
	}

	return r;
}

void Channel::registerInterruptListener(Connection& c, hal::GPIO& gpio, int8_t code)
{
	if(ThreadCtl(_NTO_TCTL_IO, 0) == -1)
	{
		MXT_THROW("Couldn't access HW!");
	}

	gpio.disableInterrupts();
	gpio.makeEdgeSensitive();
	gpio.clearInterruptFlags();

	struct sigevent e;

	SIGEV_PULSE_INIT(&e, c.mID, SIGEV_PULSE_PRIO_INHERIT, mIntCode = code, 0);

	if((mIntID = InterruptAttachEvent(MXT_HWINT, &e, 0)) == -1)
	{
		MXT_THROW("Failed to attach interrupt event!");
	}

	gpio.enableInterrupts();
}

void Channel::registerTimerListener(Connection& c, int8_t code, uint64_t period)
{
	static bool can_register = true;

	if(!can_register)
	{
		MXT_THROW_EX(TimerRedefinedException);
	}

	can_register = false;

	timer_t timerid;
	struct sigevent e;
	struct itimerspec timer;

	SIGEV_PULSE_INIT(&e, c.mID, SIGEV_PULSE_PRIO_INHERIT, code, 0);

	timer_create(CLOCK_REALTIME, &e, &timerid);

	timer.it_value.tv_sec  = timer.it_interval.tv_sec  = period / MXT_NS_IN_S;
	timer.it_value.tv_nsec = timer.it_interval.tv_nsec = period % MXT_NS_IN_S;

	timer_settime(timerid, 0, &timer, nullptr);
}

void Channel::registerADC(Connection& c, ADC& adc, int8_t code)
{
	adc.registerAdcISR(c.mID, code);
}

}}
