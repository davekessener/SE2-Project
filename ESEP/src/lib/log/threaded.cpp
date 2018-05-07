#include <fstream>

#include "lib/log/threaded.h"

namespace esep { namespace log {

Threaded::Threaded(void)
	: mOut("log.txt") // TODO
{
}

void Threaded::doWrite(const std::string& s)
{
	lock_t lock(mMutex);

	mOut << s << std::endl;
}

void Threaded::doEcho(const std::string& s)
{
	lock_t lock(mMutex);

	Base::doEcho(s);
}

}}
