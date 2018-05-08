#include <sstream>
#include <iostream>

#include "test/ft/unit.h"

#include "lib/utils.h"
#include "lib/logger.h"
#include "lib/timer.h"
#include "lib/writer.h"

#include "lib/io/stream_writer.h"

#include "test/unit/manager.h"

#include "test/ut/logger_format_parser.h"
#include "test/ut/crc32.h"
#include "test/ut/fsm.h"
#include "test/ut/byte_stream.h"
#include "test/ut/dummy_connection.h"
#include "test/ut/serial_client.h"
#include "test/ut/qnx_connections.h"
#include "test/ut/timer.h"
#include "test/ut/watchdog.h"
#include "test/ut/sync_container.h"
#include "test/ut/communication_layer.h"

namespace esep { namespace test { namespace functional {

bool runUnitTests(bool verbose)
{
	lib::Timer::instance().sleep(5);

	auto r = unit::Manager()
		.addTest<unit::CRC32>()
		.addTest<unit::LoggerFormatParser>()
		.addTest<unit::FSM>()
		.addTest<unit::ByteStream>()
		.addTest<unit::QNXConnections>()
		.addTest<unit::SyncContainer>()
		.addTest<unit::Timer>()
		.addTest<unit::DummyConnection>()
		.addTest<unit::SerialClient>()
		.addTest<unit::Watchdog>()
		.addTest<unit::CommunicationLayer>()
		.run();

	uint w = 0;
	bool success = true;

	std::cout << std::endl;

	for(const auto& t : r)
	{
		if(t.first.size() > w) w = t.first.size();
	}

	for(const auto& t : r)
	{
		uint succ = 0, total = t.second.first.size();

		if(total == 0)
		{
			std::cout << t.first << ": No tests run !!!\n";

			continue;
		}

		for(const auto& p : t.second.first)
		{
			if(p.first == unit::TestSuite::Result::SUCCESS)
			{
				++succ;
			}
		}

		uint p = succ * 100 / total;

		if(verbose)
		{
			std::cout << std::setw(w) << t.first << ": " << std::setw(3) << p << "% [" << std::setw(2) << succ << " / "
					  << std::setw(2) << total << "] tests successful\n";
		}
	}

	if(verbose)
	{
		std::cout << "\n";
	}

	for(const auto& t : r)
	{
		if(!t.second.second.empty())
		{
			std::cout << t.first << " has encountered a critical error: " << t.second.second << "!\n";
			success = false;
		}
	}

	for(const auto& t : r)
	{
		for(const auto& p : t.second.first)
		{
			if(p.first == unit::TestSuite::Result::FAILURE)
			{
				std::cout << "[ERR] " << t.first << ": " << p.second << "\n";
				success = false;
			}
		}
	}

	if(success)
	{
		std::cout << "SUCCESS!";
	}

	std::cout << std::endl;

	return success;
}

}}}
