#include <sys/neutrino.h>

#include "hal/async_channel.h"

#include "lib/utils.h"

namespace esep { namespace hal {

AsyncChannel::AsyncChannel(void)
{
	if((mChannelID = CreateChannel_r(0)) == -1)
	{
		MXT_THROW("Could not create channel");
	}

	if((mConnectionID = ConnectAttach_r(0, 0, mChannelID, 0, 0)) == -1)
	{
		MXT_THROW("Could not create connection.");
	}
}

AsyncChannel::~AsyncChannel(void)
{
	ConnectDetach(mConnectionID);
	ChannelDestroy(mChannelID);
}

AsyncChannel::message_t AsyncChannel::getMessage(void)
{
	return 0;
}

}}
