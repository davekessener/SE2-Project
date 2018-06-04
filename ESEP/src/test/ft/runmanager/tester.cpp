#include <iostream>

#include "test/ft/runmanager/tester.h"

#include "hal/hal.h"
#include "hal/buttons.h"
#include "hal/physical.h"

#include "data/location_data.h"

#include "hal.h"

namespace esep { namespace test { namespace functional { namespace r {

Tester::Tester(void)
	: mConfig("")
	, mHandler(&mConfig)
	, mRunning(false)
	, mKeep(false)
{
	mHAL.reset(new hal::Physical);

	HAL::instance().instantiate(mHAL.get(), &mConfig);

	std::cout << "=== RUN MANAGER TEST =================================================================================" << std::endl;
}

Tester::~Tester(void)
{
	HAL::instance().clear();

	std::cout << "======================================================================================================" << std::endl;
}

void Tester::run(void)
{
	typedef hal::HAL::Event Event;
	typedef hal::Buttons::Button Button;

	mHandler.setMaster(this);

	mRunning = true;

	mHAL->setCallback([this](Event e) {
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
				HAL_CONSOLE.println("Mode: \tAll items are " (mKeep.load() ? "accepted!" : "sorted out!"));
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

	mHAL->setCallback([](Event) { });
}

void Tester::accept(Packet_ptr p)
{
	HAL_CONSOLE.println("Received ", p);

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
				HAL_CONSOLE.println("Message: \t"(p->message() == Message::Run::REACHED_END ? "REACHED_END" : "ITEM_REMOVED"));
				break;

			case Message::Run::ITEM_APPEARED:
			case Message::Run::ITEM_DISAPPEARED:
				send(Message::Run::SUSPEND);
				HAL_CONSOLE.println("Error: \tITEM_", (p->message() == Message::Run::ITEM_APPEARED ? "APPEARED" : "DISAPPEARED"), " in ",
						static_cast<data::Location&>(**p->begin()).location());
				break;

			case Message::Run::RAMP_FULL:
				send(Message::Run::SUSPEND);
				HAL_CONSOLE.println("Error: \tRAMP_FULL");
				break;

			case Message::Run::ANALYSE:
				if(mKeep.load())
				{
					send(Message::Run::KEEP_NEXT);
				}
				break;

			case Message::Run::END_FREE:
				HAL_CONSOLE.println("Message: \tEND_FREE");
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
