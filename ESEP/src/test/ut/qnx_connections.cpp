#include "test/ut/qnx_connections.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace ut {

QNXConnections::QNXConnections(void)
	: TestSuite("QNX Connections")
	, mChannel(nullptr)
{
}

void QNXConnections::setup(void)
{
	mChannel = new qnx::Channel;
	mConnection = mChannel->connect();
}

void QNXConnections::teardown(void)
{
	mConnection.close();
	delete mChannel;
}

void QNXConnections::define(void)
{
	UNIT_TEST("can open a channel and connect to it")
	{
	};

	UNIT_TEST("can send a pulse msg")
	{
		qnx::pulse_t cmp;

		cmp.code = 13;
		cmp.value = 0x12345678;

		mConnection.sendPulse(cmp);

		auto r = mChannel->receivePulse();

		ASSERT_EQUALS(r.code, cmp.code);
		ASSERT_EQUALS(r.value, cmp.value);
	};
}

}}}
