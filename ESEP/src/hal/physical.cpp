#include <cstring>
#include <cerrno>

#include <sys/neutrino.h>
#include <hw/inout.h>
#include <sys/mman.h>

#include "hal/physical.h"
#include "lib/array_constructor.h"
#include "lib/logger.h"
#include "qnx/pulse.h"
#include "qnx/channel.h"

#define MXT_OUT 0
#define MXT_SET 1
#define MXT_RESET 2

#define MXT_ASS(f,fn,i) ((static_cast<uint>(f) << 24) | ((fn) << 16) | (i))

#define MXT_GETBLOCK(v) (((v)>>24)&0xff)
#define MXT_GETF(v) (((v)>>16)&0xff)
#define MXT_GETPIN(v) ((v)&0xffff)

namespace esep { namespace hal {

const uint32_t GPIO_BASE_0 = 0x44E07000; // PORT B
const uint32_t GPIO_BASE_1 = 0x4804C000; // PORT A
const uint32_t GPIO_BASE_2 = 0x481AC000; // PORT C

Physical::Physical(void)
{
	mRunning = true;
	mGPIOs[0] = nullptr;
	mGPIOs[1] = nullptr;
	mGPIOs[2] = nullptr;

	mHALThread = std::thread([this](void) {
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

			mGPIOs[2]->configureForOutput();

			buf.finalize();

			qnx::Channel channel;
			mConnection = channel.connect();

			channel.registerInterruptListener(mConnection, *mGPIOs[0], static_cast<int8_t>(Code::INTERRUPT));

			updateSensors();

			while(mRunning.load())
			{
				auto p = channel.receivePulse();

				switch(p.code)
				{
				case static_cast<int8_t>(Code::SHUTDOWN):
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
					MXT_LOG(lib::stringify("Received pulse: ", (uint)p.code, ", ", lib::hex<32>(p.value)));
					break;
				}
			}

			mConnection.close();
		}
		catch(const std::string& e)
		{
			MXT_LOG(lib::stringify("Could not initialize HAL: ", e));
		}
		catch(const std::exception& e)
		{
			MXT_LOG(lib::stringify("Could not initialize HAL: ", e.what()));
		}
		catch(...)
		{
			MXT_LOG("Could not initialize HAL: [unknown error]");
		}

		delete mGPIOs[0];
		delete mGPIOs[1];
		delete mGPIOs[2];
	});

	lib::Timer::instance().sleep(100);
}

Physical::~Physical(void)
{
	mRunning = false;
	try
	{
		mConnection.sendPulse(static_cast<int8_t>(Code::SHUTDOWN));
	}
	catch(...)
	{
		MXT_LOG("Couldn't send shutdown signal; may hang!");
	}
	mHALThread.join();
}

void Physical::updateSensors(void)
{
	update(Field::GPIO_0, mGPIOs[0]->read());
	mGPIOs[0]->clearInterruptFlags();
}

void Physical::onGPIO(uint b, gpio_fn f, uint32_t v)
{
	(mGPIOs[b]->*f)(v);
}

void Physical::out(Field f, uint32_t v)
{
	mConnection.sendPulse(qnx::pulse_t(static_cast<int8_t>(f == Field::GPIO_1 ? Code::GPIO_1_OUT : Code::GPIO_2_OUT), v));
}

void Physical::set(Field f, bitmask_t v)
{
	mConnection.sendPulse(qnx::pulse_t(static_cast<int8_t>(f == Field::GPIO_1 ? Code::GPIO_1_SET : Code::GPIO_2_SET), v));
}

void Physical::reset(Field f, bitmask_t v)
{
	mConnection.sendPulse(qnx::pulse_t(static_cast<int8_t>(f == Field::GPIO_1 ? Code::GPIO_1_RESET : Code::GPIO_2_RESET), v));
}

}}

