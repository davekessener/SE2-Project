#include "run_manager.h"
#include "lib/petri_net.h"
#include "lib/timer.h"

namespace esep { namespace base {

#define MXT_P_NR_STATES	13

RunManager::RunManager(communication::IRecipient* m)
	:	mMaster(m)
	,	mLogic(MXT_P_NR_STATES, Auto::FIRE)
{

}

RunManager::~RunManager()
{

}

void RunManager::enter()
{

}

void RunManager::leave()
{

}

void RunManager::handle(hal::HAL::Event)
{

}

void RunManager::accept(communication::Packet_ptr)
{

}

}
}
