#include "lib/observer.h"

namespace esep { namespace lib {

Observable::Observable(void)
{
	mNextID = 0;
}

Observable::~Observable(void)
{
}

Observable::id_t Observable::subscribe(callback_t cb)
{
	id_t id = mNextID++;

	mObservers[id] = cb;

	return id;
}

void Observable::unsubscribe(id_t id)
{
	auto i = mObservers.find(id);

	if(i == mObservers.end())
	{
		// TODO log errorneous unsub
	}
	else
	{
		mObservers.erase(i);
	}
}

void Observable::notifyAll(void)
{
	for(auto e : mObservers)
	{
		e.second(this);
	}
}

}}

