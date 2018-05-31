#include <iostream>

#include "test/ft/runmanager/tester.h"

#include "hal/hal.h"
#include "hal/buttons.h"
#include "hal/physical.h"

#include "data/location_data.h"

#include "system.h"

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

	mHandler.setMaster(this);

	mRunning = true;

	System::instance().HAL().setCallback([this](Event e) {
		if(mRunning.load()) switch(e)
		{
		case Event::BTN_STOP:
			if(HAL_BUTTONS.isPressed(Button::STOP))
			{
				mRunning = false;
			}
			break;

		case Event::BTN_RESET:
			if(HAL_BUTTONS.isPressed(Button::RESET))
			{
				mKeep = !mKeep.load();
			}
			break;

		default:
			mHandler.handle(e);
			break;
		}
	});

	std::cout << "Config data is " << (mConfig.isValid() ? "" : "NOT ") << "valid!" << std::endl;

	while(mRunning.load())
	{
		lib::Timer::instance().sleep(10);
	}

	System::instance().HAL().setCallback([](Event) { });
}

void Tester::accept(Packet_ptr p)
{
	HAL_CONSOLE.println("Received", p);

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
				send(Message::Base::IDLE);
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
			case Message::Run::ITEM_REMOVED:
				send(Message::Run::SUSPEND);
				break;

			case Message::Run::ITEM_APPEARED:
			case Message::Run::ITEM_DISAPPEARED:
				HAL_CONSOLE.println("Item ", (p->message() == Message::Run::ITEM_APPEARED ? "appeared" : "disappeared"), " in ",
						static_cast<data::Location&>(**p->begin()).location());
				send(Message::Error::SERIAL);
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
