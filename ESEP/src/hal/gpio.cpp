#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/mman.h>

#include "hal/gpio.h"
#include "hal/hal.h"

#include "lib/logger.h"


#define MXT_BASESIZE 0x010000
#define MXT_OE_OFFSET 0x134
#define MXT_OE_FLAGS 0xffffffc3

#define MXT_SET		0x194 //Offset for register to set output bits
#define MXT_RST		0x190 //Offset for register to clear output bits
#define MXT_READ	0x138 //Offset for register to read input bits
#define MXT_WRITE	0x13C //Offset for register to write output bits directly

#define	MXT_RISINGDETECT    	 0x148
#define	MXT_FALLINGDETECT	     0x14C

#define	MXT_LEVELDETECT(n) 		(0x140 + (n * 4))
#define	MXT_IRQSTATUS(n)  		(0x02C + (n * 4))
#define	MXT_IRQSTATUS_SET(n)	(0x034 + (n * 4))
#define	MXT_IRQSTATUS_CLR(n)	(0x03C + (n * 4))

#define MXT_GPIO_INT_LINE_1 0x0
#define MXT_GPIO_INT_LINE_2 0x1


namespace esep { namespace hal {

namespace
{
	uint32_t calcPins(void)
	{
		uint32_t r = 0;

		for(auto i1 = HAL::EVENTS, i2 = i1 + HAL::N_EVENTS ; i1 != i2 ; ++i1)
		{
			r |= static_cast<uint>(*i1);
		}

		return r;
	}

	const uint32_t bitmaskForAllSensors(void)
	{
		static uint32_t bm = calcPins();

		return bm;
	}
}

GPIO::GPIO(uint32_t a)
	: mBaseAddr(mmap_device_io(MXT_BASESIZE, a))
{
	if(mBaseAddr == MAP_DEVICE_FAILED)
	{
		MXT_THROW("Failed to map GPIO @$", lib::hex<32>(a), "!");
	}
}

GPIO::~GPIO(void)
{
	munmap_device_io(mBaseAddr, MXT_BASESIZE);
}

void GPIO::configureForOutput(void)
{
	out32(mBaseAddr + MXT_OE_OFFSET, MXT_OE_FLAGS);
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

void GPIO::makeEdgeSensitive(void)
{
	static const uint32_t pins = bitmaskForAllSensors();

	*reinterpret_cast<int *>(mBaseAddr + MXT_RISINGDETECT)   |=  pins;
	*reinterpret_cast<int *>(mBaseAddr + MXT_FALLINGDETECT)  |=  pins;
	*reinterpret_cast<int *>(mBaseAddr + MXT_LEVELDETECT(0)) &= ~pins;
	*reinterpret_cast<int *>(mBaseAddr + MXT_LEVELDETECT(1)) &= ~pins;
}

void GPIO::clearInterruptFlags(void)
{
	out32(mBaseAddr + MXT_IRQSTATUS(MXT_GPIO_INT_LINE_2), 0xFFFFFFFF);
}

void GPIO::enableInterrupts(void)
{
	static const uint32_t pins = bitmaskForAllSensors();

	out32(mBaseAddr + MXT_IRQSTATUS_SET(MXT_GPIO_INT_LINE_2), pins);
}

void GPIO::disableInterrupts(void)
{
	out32(mBaseAddr + MXT_IRQSTATUS_CLR(MXT_GPIO_INT_LINE_2), 0xFFFFFFFF);
}

}}
