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

const uint32_t GPIO_OE_FLAGS = 0xffffffc3;
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

			mGPIOs[static_cast<uint>(Field::GPIO_2)]->configure(GPIO_OE_FLAGS);

			buf.finalize();

			qnx::Channel channel;
			mConnection = channel.connect();

//			channel.listenForInterrupts(mConnection);

			while(mRunning.load())
			{
				auto p = channel.receivePulse();

				switch(p.code)
				{
				case static_cast<int8_t>(qnx::Code::SHUTDOWN):
					break;
				case static_cast<int8_t>(qnx::Code::GPIO):
						onGPIO(MXT_GETBLOCK(p.value), MXT_GETF(p.value), MXT_GETPIN(p.value));
					break;
				case static_cast<int8_t>(qnx::Code::INTERRUPT):
					onInt();
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
}

Physical::~Physical(void)
{
	mRunning = false;
	mConnection.sendPulse(qnx::Code::SHUTDOWN);
	mHALThread.join();
}

void Physical::onInt(void)
{
	update(Field::GPIO_0, mGPIOs[0]->read());
}

void Physical::onGPIO(uint b, uint f, uint p)
{
	switch(f)
	{
	case MXT_OUT:
		mGPIOs[b]->write(1 << p);
		break;
	case MXT_SET:
		mGPIOs[b]->setBits(1 << p);
		break;
	case MXT_RESET:
		mGPIOs[b]->resetBits(1 << p);
		break;
	}
}

void Physical::sendGPIOPulse(Field f, uint fn, uint v)
{
	for(uint i = 0 ; i < sizeof(v) * 8 ; ++i)
	{
		if(v & (1 << i))
		{
			mConnection.sendPulse(qnx::pulse_t(static_cast<int8_t>(qnx::Code::GPIO), MXT_ASS(f, fn, i)));
		}
	}
}

void Physical::out(Field f, uint32_t v)
{
	sendGPIOPulse(f, MXT_OUT, v);
}

void Physical::set(Field f, bitmask_t v)
{
	sendGPIOPulse(f, MXT_SET, v);
}

void Physical::reset(Field f, bitmask_t v)
{
	sendGPIOPulse(f, MXT_RESET, v);
}

}}

