#include "hal/switch.h"

#define MXT_BM_SSENSOR (1<<14)
#define MXT_BM_SWITCH (1<<19)

namespace esep { namespace hal {

Switch::Switch(HAL *hal)
	: mHAL(hal)
{
}

Switch::~Switch(void)
{
}

void Switch::open(void)
{
	mHAL->set(HAL::Field::GPIO_1, MXT_BM_SWITCH);
}

void Switch::close(void)
{
	mHAL->reset(HAL::Field::GPIO_1, MXT_BM_SWITCH);
}

bool Switch::isOpen(void) const
{
	return mHAL->in(HAL::Field::GPIO_0) & MXT_BM_SSENSOR;
}

}}
