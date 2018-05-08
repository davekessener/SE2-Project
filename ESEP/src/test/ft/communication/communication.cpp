#include <memory>
#include <iostream>

#include "test/ft/serial/watchdog.h"

#include "serial/actual_connection.h"
#include "serial/bsp_client.h"
#include "serial/watchdog.h"

#include "hal/hal.h"
#include "hal/physical.h"
#include "hal/buttons.h"
#include "hal/light_barriers.h"
#include "hal/switch.h"
#include "hal/motor.h"
#include "hal/metal_sensor.h"
#include "hal/lights.h"

#include "communication/base.h"
#include "communication/master.h"
#include "communication/slave.h"

#include "data/data_point.h"
#include "data/metalsensor_data.h"

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

struct HALWrapper
{
	HALWrapper( ) : lbs(&hal), btns(&hal), motor(&hal), swtch(&hal), metal(&hal), lights(&hal) { running = true; }
	~HALWrapper( )
	{
		swtch.close();
		motor.enable();
		motor.right();
		motor.fast();
		motor.stop();
		lights.turnOff(Lights::Light::RED);
		lights.turnOff(Lights::Light::YELLOW);
		lights.turnOff(Lights::Light::GREEN);

		MXT_LOG("HAL has been shut down.");
	}

	bool isBroken(LightBarriers::LightBarrier lb)
	{
		return lbs.isBroken(lb);
	}

	bool isPressed(Buttons::Button b)
	{
		return btns.isPressed(b);
	}

	template<typename F>
	void execute(F&& f)
	{
		hal.setCallback(std::forward<F>(f));

		while(running.load())
		{
			lib::Timer::instance().sleep(10);
		}
	}

	void start( ) { motor.start(); }
	void stop( ) { motor.stop(); }
	void open( ) { swtch.open(); }
	void close( ) { swtch.close(); }
	bool isMetal( ) const { return metal.isMetal(); }
	bool isOpen( ) const { return swtch.isOpen(); }
	void blink(Lights::Light l) { lights.flash(l, 1000); }

	Physical hal;
	LightBarriers lbs;
	Buttons btns;
	Motor motor;
	Switch swtch;
	MetalSensor metal;
	Lights lights;
	std::atomic<bool> running;
};

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

struct DummyMaster : public IRecipient
{
	void accept(Packet_ptr p) override
	{
		MXT_LOG(lib::stringify("Master received ", p->message(), " from ", getLocationName(p->source())));

		switch(p->message())
		{
		case Packet::Message::NEW_ITEM:
			if(p->source() == Packet::Location::BASE_M)
			{
				send(Packet::Location::BASE_M, Packet::Message::RESUME);
				send(Packet::Location::BASE_S, Packet::Message::SUSPEND);
			}
			else if(p->source() == Packet::Location::BASE_S)
			{
				send(Packet::Location::BASE_S, Packet::Message::RESUME);
			}
			break;

		case Packet::Message::ANALYSE:
			analyse(p);
			break;

		case Packet::Message::REACHED_END:
			if(p->source() == Packet::Location::BASE_M)
			{
				send(Packet::Location::BASE_M, Packet::Message::SUSPEND);
			}
			else if(p->source() == Packet::Location::BASE_S)
			{
				send(Packet::Location::BASE_M, Packet::Message::RESUME);
			}
			break;

		default:
			break;
		}
	}

	void send(Packet::Location dst, Packet::Message msg)
	{
		mBase->accept(Packet_ptr(new Packet(Packet::Location::MASTER, dst, msg)));
	}

	void analyse(Packet_ptr p)
	{
		bool should_keep = true;

		for(auto& dp : *p)
		{
			if(dp->type() == data::DataPoint::Type::METAL_SENSOR)
			{
				if(dynamic_cast<const data::MetalSensor&>(*dp).isMetal())
				{
					should_keep = false;
				}
			}
		}

		if(should_keep)
		{
			send(p->source(), Packet::Message::KEEP_NEXT);
		}
	}

	void setCommunication(IRecipient *r) { mBase = r; }

	IRecipient *mBase;
};

struct DummyBase : public IRecipient
{
	DummyBase(HALWrapper& hal, const std::string& name, IRecipient *master)
		: mName(name), mMaster(master), mHAL(hal) { }

	void accept(Packet_ptr p) override
	{
		if(p->message() == Packet::Message::ERROR_SERIAL)
		{
			MXT_LOG(lib::stringify("Base ", mName, " received ERROR_SERIAL message. Shutting down ..."));

			mHAL.running = false;
		}
		else
		{
			MXT_LOG(lib::stringify("Base ", mName, " received ", p->message(), " from ", getLocationName(p->source())));

			switch(p->message())
			{
			case Packet::Message::RESUME:
				mHAL.start();
				break;
			case Packet::Message::SUSPEND:
				mHAL.stop();
				break;
			case Packet::Message::KEEP_NEXT:
				mHAL.open();
				break;
			default:
				break;
			}
		}
	}

	void handle(HAL::Event e)
	{
		MXT_LOG(lib::stringify("Base ", mName, " registered event ", e, "."));

		switch(e)
		{
		case HAL::Event::LB_START:
			if(mHAL.isBroken(LightBarriers::LightBarrier::LB_START))
			{
				send(Packet::Message::NEW_ITEM);
			}
			break;
		case HAL::Event::LB_SWITCH:
			if(mHAL.isBroken(LightBarriers::LightBarrier::LB_SWITCH))
			{
				Packet_ptr p(new Packet(Packet::Location::BASE, Packet::Location::MASTER, Packet::Message::ANALYSE));

				p->addDataPoint(data::Data_ptr(new data::MetalSensor(mHAL.isMetal())));

				mMaster->accept(p);
			}
			else
			{
				mTimer = lib::Timer::instance().registerCallback([this](void) { mHAL.close(); }, 750);
			}
			break;
		case HAL::Event::LB_RAMP:
			if(mHAL.isBroken(LightBarriers::LightBarrier::LB_RAMP))
			{
				mHAL.stop();
			}
			break;
		case HAL::Event::LB_END:
			if(mHAL.isBroken(LightBarriers::LightBarrier::LB_END))
			{
				send(Packet::Message::REACHED_END);
			}
			break;
		default:
			break;
		}
	}

	void send(Packet::Message msg)
	{
		mMaster->accept(Packet_ptr(new Packet(Packet::Location::BASE, Packet::Location::MASTER, msg)));
	}

	const std::string mName;
	IRecipient * const mMaster;
	HALWrapper& mHAL;
	timer::Manager mTimer;
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

		HALWrapper hal;

		std::cout << "Connecting " << std::flush;

		while(!client->connected())
		{
			std::cout << "." << std::flush;
			lib::Timer::instance().sleep(1000);
		}

		std::cout << " [DONE]\nPlease put an item on belt 1." << std::endl;

		auto run = [&](const std::string& name, Base *m) {
			DummyBase base(hal, name, m);

			m->setBase(&base);

			hal.blink(Lights::Light::GREEN);

			hal.execute([&](HAL::Event e) {
				if(e == HAL::Event::BTN_STOP && hal.isPressed(Buttons::Button::STOP))
				{
					hal.running = false;
				}
				else
				{
					base.handle(e);
				}
			});
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
