#include <fstream>

#include "lib/log/threaded.h"

namespace esep { namespace log {

Threaded::Threaded(void)
	: mOut(new std::ofstream("log.txt")) // TODO
{
}

void Threaded::doWrite(const std::string& s)
{
	lock_t lock(mMutex);

	*mOut << s;
}

void Threaded::doEcho(const std::string& s)
{
	lock_t lock(mMutex);

	Base::doEcho(s);
}

}}
