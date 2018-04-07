#include <iostream>
#include <fstream>

#include "lib/log/standard.h"

#define MXT_LOGFILE "log.txt"

namespace esep { namespace log {

Standard::Standard(void)
	: mOS(new std::ofstream(MXT_LOGFILE))
{
}

void Standard::doWrite(const std::string& msg)
{
	*mOS << msg << std::endl;
}
}}
