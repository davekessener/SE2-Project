#include <cstring>
#include <cerrno>

#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/mman.h>

#include "hal/physical.h"
#include "lib/logger.h"
#include "qnx/pulse.h"
#include "qnx/channel.h"

namespace esep { namespace hal {

/**
 * Base addresses of the GPIO blocks
 */
constexpr uint32_t GPIO_BASE_0 = 0x44E07000; // Sensors
constexpr uint32_t GPIO_BASE_1 = 0x4804C000; // Actors /wo LEDs
constexpr uint32_t GPIO_BASE_2 = 0x481AC000; // LEDs


Physical::Physical(void)
{
	mRunning = false;
	mThreadAlive = true;

	mHALThread.construct([this](void) {
		try
		{
			if(ThreadCtl(_NTO_TCTL_IO_PRIV, 0) == -1)
			{
				MXT_THROW("Failed to aquire HW rights via ThreadCtl!");
			}

			mGPIOs[0].reset(new GPIO(GPIO_BASE_0));
			mGPIOs[1].reset(new GPIO(GPIO_BASE_1));
			mGPIOs[2].reset(new GPIO(GPIO_BASE_2));

			mGPIOs[2]->configureForOutput();

			qnx::Channel channel;

			mConnection = channel.connect();
			// Send a Code::INTERRUPT pulse msg when an interrupt in GPIO Block 0 occurs
			channel.registerInterruptListener(mConnection, *mGPIOs[0], static_cast<int8_t>(Code::INTERRUPT));

			updateSensors();

			mRunning = true;
			while(mRunning.load())
			{
				auto p = channel.receivePulse();

				switch(p.code)
				{
				case static_cast<int8_t>(Code::SHUTDOWN):
					mRunning = false;
					break;

				case static_cast<int8_t>(Code::GPIO_1_OUT):
					onGPIO(1, &GPIO::write, p.value);
					break;

				case static_cast<int8_t>(Code::GPIO_2_OUT):
					onGPIO(2, &GPIO::write, p.value);
					break;

				case static_cast<int8_t>(Code::GPIO_1_SET):
					onGPIO(1, &GPIO::setBits, p.value);
					break;

				case static_cast<int8_t>(Code::GPIO_2_SET):
					onGPIO(2, &GPIO::setBits, p.value);
					break;

				case static_cast<int8_t>(Code::GPIO_1_RESET):
					onGPIO(1, &GPIO::resetBits, p.value);
					break;

				case static_cast<int8_t>(Code::GPIO_2_RESET):
					onGPIO(2, &GPIO::resetBits, p.value);
					break;

				case static_cast<int8_t>(Code::INTERRUPT):
					updateSensors();
					break;

				default:
					MXT_LOG(lib::stringify("Received pulse {", lib::hex<8>(p.code), ", ", lib::hex<32>(p.value), "}"));
					break;
				}
			}
		}
		MXT_CATCH_ALL_STRAY

		mConnection.close();
		mThreadAlive = false;
	});

	// Wait until the thread has either reached it's main loop
	// or died and quit.
	while(mThreadAlive.load() && !mRunning.load())
	{
		lib::Timer::instance().sleep(1);
	}
}

Physical::~Physical(void)
{
	try
	{
		mConnection.sendPulse(static_cast<int8_t>(Code::SHUTDOWN));
	}
	catch(...)
	{
		mRunning = false;

		MXT_LOG("Couldn't send shutdown signal; may hang!");
	}
}

void Physical::updateSensors(void)
{
	try
	{
		update(Field::GPIO_0, mGPIOs[0]->read());
		mGPIOs[0]->clearInterruptFlags();
	}
	MXT_CATCH_ALL_STRAY
}

void Physical::onGPIO(uint b, gpio_fn f, uint32_t v)
{
	((mGPIOs[b].get())->*f)(v); // calls the GPIO member function 'f' on GPIO object #b
}

void Physical::out(Field f, uint32_t v)
{
	if(f != Field::GPIO_1 && f != Field::GPIO_2)
		MXT_THROW(lib::stringify("Tried to write ", lib::hex<32>(v), " to ", static_cast<uint>(f), "!"));

	mConnection.sendPulse(qnx::pulse_t(static_cast<int8_t>(f == Field::GPIO_1 ? Code::GPIO_1_OUT : Code::GPIO_2_OUT), v));
}

void Physical::set(Field f, bitmask_t v)
{
	if(f != Field::GPIO_1 && f != Field::GPIO_2)
		MXT_THROW(lib::stringify("Tried to set bits ", lib::hex<32>(v), " to ", static_cast<uint>(f), "!"));

	mConnection.sendPulse(qnx::pulse_t(static_cast<int8_t>(f == Field::GPIO_1 ? Code::GPIO_1_SET : Code::GPIO_2_SET), v));
}

void Physical::reset(Field f, bitmask_t v)
{
	if(f != Field::GPIO_1 && f != Field::GPIO_2)
		MXT_THROW(lib::stringify("Tried to reset bits ", lib::hex<32>(v), " to ", static_cast<uint>(f), "!"));

	mConnection.sendPulse(qnx::pulse_t(static_cast<int8_t>(f == Field::GPIO_1 ? Code::GPIO_1_RESET : Code::GPIO_2_RESET), v));
}

}}

