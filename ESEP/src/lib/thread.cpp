#include <iostream>

#include "lib/thread.h"

#include "lib/utils.h"

namespace esep { namespace lib {

Thread::Thread(void)
{
}

Thread::Thread(Thread&& t)
	: mThread(std::move(t.mThread))
{
}

Thread::~Thread(void)
{
	join();
}

Thread& Thread::operator=(Thread&& t)
{
	join();

	mThread = std::move(t.mThread);

	return *this;
}

void Thread::join(void)
{
	if(mThread.joinable())
	{
		mThread.join();
	}
}

bool Thread::active(void) const
{
	return mThread.get_id() == std::this_thread::get_id();
}

void Thread::logCreation(void)
{
}

}}
