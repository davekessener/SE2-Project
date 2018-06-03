
#include <vector>
#include <iostream>
#include <atomic>

#include "test/ft/configmanager.h"

#include "hal/physical.h"

#include "base/config_manager.h"
#include "base/config_object.h"
#include "communication/IRecipient.h"

#include "lib/timer.h"
#include "hal.h"

namespace esep { namespace test { namespace functional {

typedef communication::Packet_ptr Packet_ptr;
typedef hal::HAL::Event Event;
typedef hal::Buttons::Button Button;
typedef communication::Message Message;

struct BasicRecipient : public communication::IRecipient
{
	void accept(Packet_ptr p) override
	{
		packets.push_back(p);
	}

	std::vector<Packet_ptr> packets;
};


void testConfigManager(const lib::Arguments&)
{
	std::atomic<bool> running(true);

	hal::Physical hal;
	base::ConfigObject config("configmanager-ft.conf");
	BasicRecipient handler;
	base::ConfigManager manager(&handler,&config);

	HAL::instance().clear();
	HAL::instance().instantiate(&hal, &config);

	std::cout << "=== CONFIGMANAGER TEST ======================================================================" << std::endl;

	hal.setCallback([&](Event e) {

		switch (e)
		{
		case Event::BTN_START:
			if (HAL_BUTTONS.isPressed(Button::START))
			{
				manager.enter();
				std::cout << "[START] Place an Item at the beginning of the Module" << std::endl;
			}
			break;

		case Event::BTN_STOP:
			if (HAL_BUTTONS.isPressed(Button::STOP))
			{
				manager.leave();
				running = false;
				std::cout << "[STOP] Bye!" << std::endl;
			}
			break;

		default:
			manager.handle(e);
			break;
		}

	});

	while(running.load())
	{
		if (handler.packets.size() > 0)
		{
			if (handler.packets.front()->message() == Message::Config::DONE)
			{
				manager.leave();
				std::cout << "Measured Data:" << std::endl;
				std::cout << "----------------------------------" << std::endl;
				std::cout << "Start to HS     0x" << lib::hex<16>(config.startToHs()) << std::endl;
				std::cout << "HS to Switch    0x" << lib::hex<16>(config.hsToSwitch()) << std::endl;
				std::cout << "Switch to End:  0x" << lib::hex<16>(config.switchToEnd()) << std::endl;
				std::cout << "Slow factor:    " << config.slowFactor() << std::endl;
				std::cout << "Time Tolerance: " << config.timeTolerance() << std::endl;
				std::cout << "----------------------------------" << std::endl;
				std::cout << "Press [START] for another Measurement..." << std::endl;
			}
			else if (handler.packets.front()->message() == Message::Config::FAILED)
			{
				manager.leave();
				std::cout << "Failed Configuration!" << std::endl;
				running = false;
			}
			handler.packets.pop_back();
		}

		// Sleep while running
		lib::Timer::instance().sleep(20);
	}


	hal.setCallback([](Event e) {});

	std::cout << "============================================================================================" << std::endl;
}

}}}
