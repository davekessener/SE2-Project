#include <data/run_manager_timer_data.h>
#include "data/data_manager.h"

namespace esep { namespace data {

void RunManagerTimer::doSerialize(lib::ByteStream& bs) const
{
	bs << mEvent;
}

Data_ptr RunManagerTimer::deserialize(lib::ByteStream& bs)
{
	if(bs.empty())
	{
		MXT_THROW_EX(DataManager::UnexpectedEOSException);
	}

	return Data_ptr(new RunManagerTimer(static_cast<TimerEvent> (bs.remove())));
}

}}


