#include <cstring>
#include <cerrno>

#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/mman.h>

#include "hal/physical.h"
#include "lib/array_constructor.h"

namespace esep { namespace hal {

const uint32_t GPIO_OE_FLAGS = 0xffffffc3;
const uint32_t GPIO_BASE_0 = 0x44E07000; // PORT B
const uint32_t GPIO_BASE_1 = 0x4804C000; // PORT A
const uint32_t GPIO_BASE_2 = 0x481AC000; // PORT C

Physical::Physical(void)
{
	try
	{
		lib::ArrayConstructor<GPIO> buf(mGPIOs);

		if(ThreadCtl(_NTO_TCTL_IO_PRIV, 0) == -1)
		{
			throw lib::stringify("Error in ThreadCtl!");
		}

		buf.add(new GPIO(GPIO_BASE_0));
		buf.add(new GPIO(GPIO_BASE_1));
		buf.add(new GPIO(GPIO_BASE_2));

		mGPIOs[static_cast<uint>(Field::GPIO_2)]->configure(GPIO_OE_FLAGS);

		buf.finalize();
	}
	catch(const std::string& e)
	{
		throw lib::stringify(e, ": ", strerror(errno));
	}
}

Physical::~Physical(void)
{
	for(auto& p : mGPIOs)
	{
		delete p;
	}
}

void Physical::subscribeEvent(Event e, callback_t f)
{
}

void Physical::out(Field f, uint32_t v)
{
	mGPIOs[static_cast<uint>(f)]->write(v);
}

void Physical::set(Field f, bitmask_t bm)
{
	mGPIOs[static_cast<uint>(f)]->setBits(bm);
}

void Physical::reset(Field f, bitmask_t bm)
{
	mGPIOs[static_cast<uint>(f)]->resetBits(bm);
}

}}

