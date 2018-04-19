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
	if(mThread.joinable())
	{
		mThread.join();
	}
}

Thread& Thread::operator=(Thread&& t)
{
	if(mThread.joinable())
	{
		mThread.join();
	}

	mThread = std::move(t.mThread);

	return *this;
}

void Thread::logCreation(void)
{
	std::cout << lib::stringify("Just spawned a new thread with ID ", mThread.get_id()) << std::endl;
}

}}
