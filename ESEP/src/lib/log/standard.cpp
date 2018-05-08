#include <iostream>
#include <fstream>

#include "lib/log/standard.h"

#include "lib/io/file_writer.h"

#define MXT_LOGFILE "log.txt"

namespace esep { namespace log {

Standard::Standard(void)
	: mOS(MXT_LOGFILE)
{
}

void Standard::doWrite(const std::string& msg)
{
	mOS << msg << std::endl;
}
}}
