#include <iostream>

#include "test/ft/runmanager/tester.h"

#include "hal/hal.h"
#include "hal/buttons.h"
#include "hal/physical.h"

namespace esep { namespace test { namespace functional { namespace r {

Tester::Tester(void)
	: mHandler(&mConfig)
	, mRunning(false)
	, mKeep(false)
{
	std::cout << "=== RUN MANAGER TEST =================================================================================" << std::endl;
}

Tester::~Tester(void)
{
	std::cout << "======================================================================================================" << std::endl;
}

void Tester::run(void)
{
	typedef hal::HAL::Event Event;
	typedef hal::Buttons::Button Button;

	hal::Physical hal;
	hal::Buttons btns(&hal);

	mHandler.setMaster(this);

	mRunning = true;

	hal.setCallback([this, &btns](Event e) {
		if(mRunning.load()) switch(e)
		{
		case Event::BTN_STOP:
			if(btns.isPressed(Button::STOP))
			{
				mRunning = false;
			}
			break;

		case Event::BTN_RESET:
			if(btns.isPressed(Button::RESET))
			{
				mKeep = !mKeep.load();
			}
			break;

		default:
			mHandler.handle(e);
			break;
		}
	});

	while(mRunning.load())
	{
		lib::Timer::instance().sleep(10);
	}

	hal.setCallback([](Event) { });
}

void Tester::accept(Packet_ptr p)
{
	std::cout << "Received {" << (int)p->source() << " -> " << (int)p->target() << ": " << lib::hex<16>(p->message()) << "}" << std::endl;

	if(mRunning.load())
	{
		if(p->message().is<Message::Master>())
			switch(p->message().as<Message::Master>())
		{
			case Message::Master::IDLE:
				send(Message::Base::IDLE);
				break;

			case Message::Master::READY:
				send(Message::Base::READY);
				break;

			case Message::Master::RUN:
				send(Message::Base::RUN);
				break;

			case Message::Master::CONFIG:
				send(Message::Base::CONFIG);
				break;

			case Message::Master::VALID:
				send(Message::Base::VALID);
				break;

			default:
				break;
		}
		else if(p->message().is<Message::Config>())
			switch(p->message().as<Message::Config>())
		{
			case Message::Config::START:
				send(Message::Run::RESUME);
				break;

			case Message::Config::DONE:
				send(Message::Run::SUSPEND);
				break;

			default:
				break;
		}
		else if(p->message().is<Message::Run>())
			switch(p->message().as<Message::Run>())
		{
			case Message::Run::NEW_ITEM:
				send(Message::Run::RESUME);
				break;

			case Message::Run::REACHED_END:
				send(Message::Run::SUSPEND);
				break;

			case Message::Run::ANALYSE:
				if(mKeep.load())
				{
					send(Message::Run::KEEP_NEXT);
				}
				break;

			default:
				break;
		}
	}
}

void Tester::send(msg_t msg)
{
	mHandler.accept(std::make_shared<Packet>(Location::MASTER, Location::BASE, msg));
}

}}}}
