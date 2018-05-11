#include <deque>

#include "test/ut/emp_playback.h"

#include "test/unit/assertions.h"

#include "hal/hal.h"

#include "emp/playback.h"

#include "lib/io/container_reader.h"

namespace esep { namespace test { namespace unit {

typedef std::unique_ptr<hal::HAL> HAL_ptr;
typedef std::deque<std::string> buf_t;
typedef hal::HAL::Event Event;

namespace
{
	inline lib::Reader_ptr reader(const buf_t& b) { return lib::Reader_ptr(new lib::ContainerReader<buf_t>(b)); }

	buf_t default_buf( )
	{
		return buf_t{
			"0 heightsensor $1234 # heighsensor",
			"# empty line",
			"",
			"50 lb_end 1",
			"100 lb_start 0",
			"150 button_stop true",
			"200 button_reset true"
		};
	}
}

EMPPlayback::EMPPlayback(void)
	: TestSuite("EMP Playback HAL")
{
}

void EMPPlayback::define(void)
{
	UNIT_TEST("can be created and destroyed")
	{
		buf_t buf;
		HAL_ptr hal(new emp::Playback(reader(buf)));
	};

	UNIT_TEST("can parse sample file")
	{
		buf_t buf = default_buf();
		lib::Timer::instance().reset();
		HAL_ptr hal(new emp::Playback(reader(buf)));
	};

	UNIT_TEST("sends events in correct order")
	{
		std::vector<Event> received;

		received.reserve(10);

		buf_t buf = default_buf();
		lib::Timer::instance().reset();
		HAL_ptr hal(new emp::Playback(reader(buf)));

		hal->setCallback([&received](Event e) {
			received.push_back(e);
		});

		lib::Timer::instance().sleep(300);

		ASSERT_EACH_EQUALS(received, (decltype(received){Event::LB_START, Event::BTN_RESET}));
	};

	UNIT_TEST("sends events with correct timing")
	{
		std::vector<Event> received;

		received.reserve(10);

		buf_t buf = default_buf();
		lib::Timer::instance().reset();
		HAL_ptr hal(new emp::Playback(reader(buf)));

		hal->setCallback([&received](Event e) {
			received.push_back(e);
		});

		lib::Timer::instance().sleep(50);

		ASSERT_EQUALS(received.size(), 0u);

		lib::Timer::instance().sleep(100);

		ASSERT_EQUALS(received.size(), 1u);

		lib::Timer::instance().sleep(100);

		ASSERT_EQUALS(received.size(), 2u);
		ASSERT_EACH_EQUALS(received, (decltype(received){Event::LB_START, Event::BTN_RESET}));
	};
}

}}}
