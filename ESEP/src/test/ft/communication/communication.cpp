#include <memory>
#include <iostream>

#include "test/ft/serial/watchdog.h"

#include "serial/actual_connection.h"
#include "serial/bsp_client.h"
#include "serial/watchdog.h"

#include "hal/hal.h"
#include "hal/physical.h"
#include "hal/buttons.h"

#include "communication/base.h"
#include "communication/master.h"
#include "communication/slave.h"

#include "lib/logger.h"

#define MXT_SERIAL_DEVICE "/dev/ser4"

#define MXT_ARGS_M "master"
#define MXT_ARGS_S "slave"

namespace esep { namespace test { namespace functional {

using namespace communication;
using namespace hal;

typedef std::unique_ptr<serial::Connection> connection_ptr;
typedef std::unique_ptr<serial::Client> client_ptr;
typedef std::unique_ptr<IRecipient> Recipient_ptr;
typedef serial::Client::buffer_t buffer_t;
typedef serial::Connection::ConnectionClosedException ConnectionClosedException;

// Helper functions that convert between std::string and Client::buffer_t
inline buffer_t buffer(const std::string& s) { return    buffer_t(s.cbegin(), s.cend()); }
inline std::string str(const buffer_t& b)    { return std::string(b.cbegin(), b.cend()); }

std::string getLocationName(Packet::Location l)
{
	switch(l)
	{
	case Packet::Location::MASTER:
		return "MASTER";
	case Packet::Location::BASE:
		return "BASE";
	case Packet::Location::BASE_M:
		return "BASE_M";
	case Packet::Location::BASE_S:
		return "BASE_S";
	default:
		return "[UNKNOWN]";
	}
}

Packet::Message getMessageFromEvent(HAL::Event e)
{
	switch(e)
	{
	case HAL::Event::LB_START:
		return Packet::Message::ANALYSE;
	case HAL::Event::LB_END:
		return Packet::Message::CONFIG_DONE;
	case HAL::Event::LB_HEIGHTSENSOR:
		return Packet::Message::CONFIG_FAILED;
	case HAL::Event::LB_SWITCH:
		return Packet::Message::ERROR_SERIAL;
	case HAL::Event::LB_RAMP:
		return Packet::Message::IDLE;
	default:
		return Packet::Message::ITEM_APPEARED;
	}
}

struct DummyMaster : public IRecipient
{
	void accept(Packet_ptr p) override
	{
		MXT_LOG(lib::stringify("Master received ", p->message(), " from ", getLocationName(p->source())));

		p->target(Packet::Location::BASE);
		p->source(Packet::Location::MASTER);

		mBase->accept(p);
	}

	void setCommunication(IRecipient *r) { mBase = r; }

	IRecipient *mBase;
};

struct DummyBase : public IRecipient
{
	DummyBase(std::atomic<bool>& running, const std::string& name, IRecipient *master)
		: mName(name), mMaster(master), mRunning(running) { }

	void accept(Packet_ptr p) override
	{
		if(p->message() == Packet::Message::ERROR_SERIAL)
		{
			MXT_LOG(lib::stringify("Base ", mName, " received ERROR_SERIAL message. Shutting down ..."));

			mRunning = false;
		}
		else
		{
			MXT_LOG(lib::stringify("Base ", mName, " received ", p->message(), " from ", getLocationName(p->source())));
		}
	}

	void handle(HAL::Event e)
	{
		auto msg = getMessageFromEvent(e);

		MXT_LOG(lib::stringify("Base ", mName, " registered event ", e, " and notifies master with message ", msg, "."));

		mMaster->accept(Packet_ptr(new Packet(Packet::Location::BASE, Packet::Location::MASTER, msg)));
	}

	const std::string mName;
	IRecipient * const mMaster;
	std::atomic<bool>& mRunning;
};


void testCommunicationLayer(const lib::args_t& args)
{
	std::cout << "# === COMMUNICATION LAYER TEST ===============================================================\n";

	if(args.size() != 1 || (args.front() != MXT_ARGS_M && args.front() != MXT_ARGS_S))
	{
		std::cout << "Invalid arguments!" << std::endl;
	}
	else
	{
		connection_ptr con(new serial::ActualConnection(MXT_SERIAL_DEVICE));
		client_ptr     bsp(new serial::BSPClient(std::move(con), 50));
		client_ptr  client(new serial::Watchdog(std::move(bsp), 100));

		std::atomic<bool> mRunning;

		mRunning = true;

		std::cout << "Connecting " << std::flush;

		while(!client->connected())
		{
			std::cout << "." << std::flush;
			lib::Timer::instance().sleep(1000);
		}

		std::cout << " [DONE]" << std::endl;

		Physical hal;
		Buttons btns(&hal);

		auto run = [&](const std::string& name, Base *m) {
			DummyBase base(mRunning, name, m);

			m->setBase(&base);

			hal.setCallback([&](HAL::Event e) {
				if(e == HAL::Event::BTN_STOP && btns.isPressed(Buttons::Button::STOP))
				{
					mRunning = false;
				}
				else
				{
					base.handle(e);
				}
			});

			while(mRunning.load())
			{
				lib::Timer::instance().sleep(10);
			}
		};

		if(args.front() == MXT_ARGS_M)
		{
			DummyMaster master;
			Master master_com(&master, std::move(client));

			master.setCommunication(&master_com);

			run("BASE_M", &master_com);
		}
		else
		{
			Slave slave_com(std::move(client));

			run("BASE_S", &slave_com);
		}
	}

	std::cout << "# ============================================================================================" << std::endl;
}

}}}
