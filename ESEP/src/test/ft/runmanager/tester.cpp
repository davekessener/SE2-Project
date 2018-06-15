#include <iostream>

#include "test/ft/runmanager/tester.h"

#include "hal/hal.h"
#include "hal/buttons.h"
#include "hal/physical.h"

#include "data/location_data.h"

#include "hal.h"

namespace esep { namespace test { namespace functional { namespace r {

Tester::Counter& Tester::Counter::operator++( )
{
	++v;

	HAL_CONSOLE.println("Items: ", v);

	return *this;
}

Tester::Counter& Tester::Counter::operator--( )
{
	--v;

	HAL_CONSOLE.println("Items: ", v);

	return *this;
}

Tester::Tester(void)
	: mConfig("system.conf")
	, mRunning(false)
	, mKeep(false)
	, mItemCount(0)
{
	mHAL.reset(new hal::Physical);

	HAL::instance().instantiate(mHAL.get(), &mConfig);

	std::cout << "=== RUN MANAGER TEST =================================================================================" << std::endl;
}

Tester::~Tester(void)
{
	mHandler.reset();
	HAL::instance().clear();

	std::cout << "======================================================================================================" << std::endl;
}

void Tester::run(void)
{
	typedef hal::HAL::Event Event;
	typedef hal::Buttons::Button Button;

	mHandler.reset(new base::Handler(&mConfig));

	mHandler->setMaster(this);
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
				HAL_CONSOLE.println("Mode: \tAll items are ", (mKeep.load() ? "accepted!" : "sorted out!"));
			}
			break;

		default:
			mHandler->handle(e);
			break;
		}
	});

	std::cout << "Config data is " << (mConfig.isValid() ? "" : "NOT ") << "valid!" << std::endl;

	HAL_CONSOLE.setCallback([this](const std::string& line) {
		if(line == "reset")
		{
			HAL_CONSOLE.println("Resetting!");

			mHandler.reset(new base::Handler(&mConfig));
			mHandler->setMaster(this);
			mItemCount = 0;
		}
	});

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
				mItemCount++;
				break;

			case Message::Run::REACHED_END:
				send(Message::Run::SUSPEND);
				break;

			case Message::Run::ITEM_REMOVED:
				if(!--mItemCount)
					send(Message::Run::SUSPEND);
				break;

			case Message::Run::ITEM_APPEARED:
				send(Message::Run::SUSPEND);
				break;

			case Message::Run::ITEM_DISAPPEARED:
				send(Message::Run::SUSPEND);
				mItemCount--;
				break;

			case Message::Run::RAMP_FULL:
				send(Message::Run::SUSPEND);
				break;

			case Message::Run::ITEM_STUCK:
				send(Message::Run::SUSPEND);
				mItemCount--;
				break;

			case Message::Run::ANALYSE:
				if(mKeep.load())
				{
					send(Message::Run::KEEP_NEXT);
				}
				break;

			case Message::Run::END_FREE:
				if(--mItemCount > 0)
					send(Message::Run::RESUME);
				break;

			default:
				break;

		}
	}
}

void Tester::send(msg_t msg)
{
	mHandler->accept(std::make_shared<Packet>(Location::MASTER, Location::BASE, msg));
}

}}}}
