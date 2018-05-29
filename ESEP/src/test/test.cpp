#include <iostream>

#include "test/test.h"

#include "test/ft/unit.h"
#include "test/ft/serial.h"
#include "test/ft/hal.h"
#include "test/ft/communication.h"
#include "test/ft/emp.h"
#include "test/ft/runmanager.h"

#include "lib/logger.h"

namespace esep { namespace test {

typedef void (*test_fn)(const lib::Arguments&);
typedef std::map<std::string, test_fn> tests_t;

test_fn getTest(const std::string& id)
{
	static tests_t t;

	if(t.empty())
	{
		t["serial"] = &functional::testSerialConnection;
		t["hal"] = &functional::testHAL;
		t["com"] = &functional::testCommunicationLayer;
		t["emp"] = &functional::testEMP;
		t["run"] = &functional::testRunManager;
	}

	auto i = t.find(id);

	if(i == t.end())
	{
		throw std::runtime_error(lib::stringify("Unknown test '", id, "'!"));
	}

	return i->second;
}

bool main(const lib::Arguments& args)
{
	bool ut = functional::runUnitTests(args.has("verbose"));

	if(ut && args.has("test"))
	{
		getTest(args.get("test"))(args);
	}

	return ut;
}

}}
