#include "master/item.h"

namespace esep { namespace master {

id_t Item::NEXT_ID = 0;

Item::Item(void)
	: mID(NEXT_ID++)
	, mTime(0)
	, mAction(Plugin::Action::VOID)
	, mPlugin(nullptr)
	, mLocation(Location::BASE)
{
}

}}
