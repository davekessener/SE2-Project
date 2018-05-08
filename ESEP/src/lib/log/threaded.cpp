#include <fstream>

#include "lib/log/threaded.h"

#include "lib/logger.h"

namespace esep { namespace log {

Threaded::Threaded(void)
	: mOut(new std::ofstream("log.txt")) // TODO
{
}

void Threaded::doWrite(const std::string& s)
{
	MXT_SYNCHRONIZE;

	*mOut << s;
}

void Threaded::doEcho(const std::string& s)
{
	MXT_SYNCHRONIZE;

	Base::doEcho(s);
}

}}
