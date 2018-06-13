#include <sstream>
#include <iostream>
#include <memory>

#include "test/ft/unit.h"

#include "lib/utils.h"
#include "lib/logger.h"
#include "lib/timer.h"

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
#include "test/ut/hw_location.h"
#include "test/ut/emp_tokenizer.h"
#include "test/ut/emp_parser.h"
#include "test/ut/emp_playback.h"
#include "test/ut/compound_enum.h"
#include "test/ut/master_logic.h"
#include "test/ut/config_object.h"
#include "test/ut/run_manager_time_ctrl.h"
#include "test/ut/run_manager_logic.h"
#include "test/ut/test_hal.h"
#include "test/ut/error_manager_logic.h"
#include "test/ut/process_tree.h"
#include "test/ut/overflow_buffer.h"
#include "test/ut/fun_chain.h"
#include "test/ut/bdscan.h"
#include "test/ut/stream.h"
#include "test/ut/analyser_expand.h"
#include "test/ut/any.h"
#include "test/ut/plugins.h"
#include "test/ut/plugins_sortable.h"
#include "system.h"

namespace esep { namespace test { namespace functional {

bool runUnitTests(bool verbose)
{
	lib::Timer::instance().sleep(5);

	auto r = unit::Manager()
		.addTest<ut::CRC32>()
		.addTest<ut::LoggerFormatParser>()
		.addTest<ut::FSM>()
		.addTest<ut::ByteStream>()
		.addTest<ut::QNXConnections>()
		.addTest<ut::SyncContainer>()
		.addTest<ut::Timer>()
		.addTest<ut::DummyConnection>()
		.addTest<ut::SerialClient>()
		.addTest<ut::Watchdog>()
		.addTest<ut::CommunicationLayer>()
		.addTest<ut::HWLocation>()
		.addTest<ut::EMPTokenizer>()
		.addTest<ut::EMPParser>()
		.addTest<ut::EMPPlayback>()
		.addTest<ut::CompoundEnum>()
//		.addTest<ut::MasterLogic>()
		.addTest<ut::ConfigObject>()
		.addTest<ut::TestHAL>()
		.addTest<ut::ErrorManagerLogic>()
		.addTest<ut::RunManagerTimeCtrl>()
		//.addTest<ut::RunManagerLogic>()
		.addTest<ut::ProcessTree>()
		.addTest<ut::Processor>()
		.addTest<ut::OverflowBuffer>()
		.addTest<ut::FunChain>()
		.addTest<ut::BDSCAN>()
		.addTest<ut::Stream>()
		.addTest<ut::AnalyserExpand>()
		.addTest<ut::Any>()
		.addTest<ut::Plugins>()
//		.addTest<ut::PluginSortable>()
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
			std::cout << t.first << ": No tests run !!!" << std::endl;

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
			std::cout << std::setw(w) << t.first << ": " << std::setw(3) << p << "% ["
					  << std::setw(2) << succ << " / " << std::setw(2) << total
					  << "] tests successful" << std::endl;
		}
	}

	if(verbose)
	{
		std::cout << std::endl;
	}

	for(const auto& t : r)
	{
		if(!t.second.second.empty())
		{
			std::cout << t.first << " has encountered a critical error: " << t.second.second << "!" << std::endl;

			success = false;
		}
	}

	for(const auto& t : r)
	{
		for(const auto& p : t.second.first)
		{
			if(p.first == unit::TestSuite::Result::FAILURE)
			{
				std::cout << "[ERR] " << t.first << ": " << p.second << std::endl;

				success = false;
			}
		}
	}

	if(success)
	{
		std::cout << "SUCCESS!" << std::endl;
	}

	return success;
}

}}}
