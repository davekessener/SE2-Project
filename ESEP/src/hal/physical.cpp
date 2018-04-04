#include <cstring>
#include <cerrno>

#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/mman.h>

#include "hal/physical.h"

#define MXT_BASESIZE 0x010000
#define MXT_OE_FLAGS 0xffffffc3
#define MXT_OE_OFFSET 0x134

#define MXT_SET		0x194 //Offset for register to set output bits
#define MXT_RST		0x190 //Offset for register to clear output bits
#define MXT_READ	0x138 //Offset for register to read input bits
#define MXT_WRITE	0x13C //Offset for register to write output bits directly

namespace esep { namespace hal {

const uint32_t GPIO_BASES[] = {
	0x4804C000, // PORT A (GPIO1)
	0x44E07000, // PORT B (GPIO0)
	0x481AC000  // PORT C (GPIO2)
};

Physical::Physical(void)
{
	try
	{
		if(ThreadCtl(_NTO_TCTL_IO_PRIV, 0) == -1)
		{
			throw lib::stringify("Error in ThreadCtl!");
		}

		for(uint i = 0 ; i < MXT_NPORTS ; ++i)
		{
			mBaseAddr[i] = MAP_DEVICE_FAILED;
		}

		for(uint i = 0 ; i < MXT_NPORTS ; ++i)
		{
			if((mBaseAddr[i] = mmap_device_io(MXT_BASESIZE, GPIO_BASES[i])) == MAP_DEVICE_FAILED)
			{
				throw lib::stringify("Unable to map GPIO port ", i, "!");
			}
		}

		out32(mBaseAddr[static_cast<uint>(Port::C)] + MXT_OE_OFFSET, MXT_OE_FLAGS);
	}
	catch(const std::string& e)
	{
		for(const auto& a : mBaseAddr)
		{
			if(a != MAP_DEVICE_FAILED)
			{
				munmap_device_io(a, MXT_BASESIZE);
			}
		}

		throw lib::stringify(e, ": ", strerror(errno));
	}
}

void Physical::subscribeEvent(Event e, callback_t f)
{
}

void Physical::out(Port p, uint32_t v)
{
	out32(mBaseAddr[static_cast<uint>(p)] + MXT_WRITE, v);
}

void Physical::setBits(Port p, uint32_t v)
{
	out32(mBaseAddr[static_cast<uint>(p)] + MXT_SET, v);
}

void Physical::resetBits(Port p, uint32_t v)
{
	out32(mBaseAddr[static_cast<uint>(p)] + MXT_RST, v);
}

}}

