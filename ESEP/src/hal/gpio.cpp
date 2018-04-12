#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/mman.h>

#include "hal/gpio.h"
#include "hal/hal.h"

#define MXT_BASESIZE 0x010000
#define MXT_OE_OFFSET 0x134

#define MXT_SET		0x194 //Offset for register to set output bits
#define MXT_RST		0x190 //Offset for register to clear output bits
#define MXT_READ	0x138 //Offset for register to read input bits
#define MXT_WRITE	0x13C //Offset for register to write output bits directly

#define	MXT_LEVELDETECT(n) 	(0x140 + (n * 4))
#define	MXT_RISINGDETECT    0x148
#define	MXT_FALLINGDETECT   0x14C

namespace esep { namespace hal {

namespace
{
	uint32_t calcPins(void)
	{
		uint32_t r = 0;

		for(auto i1 = HAL::EVENTS, i2 = i1 + HAL::N_EVENTS ; i1 != i2 ; ++i1)
		{
			r |= 1 << static_cast<uint>(*i1);
		}

		return r;
	}
}

GPIO::GPIO(uint32_t a)
	: mBaseAddr(mmap_device_io(MXT_BASESIZE, a))
{
	if(mBaseAddr == MAP_DEVICE_FAILED)
	{
		throw lib::stringify("Failed to map @$", lib::hex<32>(a), "!");
	}
}

GPIO::~GPIO(void)
{
	munmap_device_io(mBaseAddr, MXT_BASESIZE);
}

void GPIO::configure(uint32_t bm)
{
	out32(mBaseAddr + MXT_OE_OFFSET, bm);
}

void GPIO::write(uint32_t v)
{
	out32(mBaseAddr + MXT_WRITE, v);
}

uint32_t GPIO::read( )
{
	return in32(mBaseAddr + MXT_READ);
}

void GPIO::setBits(uint32_t bm)
{
	out32(mBaseAddr + MXT_SET, bm);
}

void GPIO::resetBits(uint32_t bm)
{
	out32(mBaseAddr + MXT_RST, bm);
}

void GPIO::configureInt(void)
{
	static const uint32_t pins = calcPins();

	*(int*)(mBaseAddr + MXT_RISINGDETECT) |= pins;
	*(int*)(mBaseAddr + MXT_FALLINGDETECT) |= pins;
	*(int*)(mBaseAddr + MXT_LEVELDETECT(0)) &= ~pins;
	*(int*)(mBaseAddr + MXT_LEVELDETECT(1)) &= ~pins;
}

}}
