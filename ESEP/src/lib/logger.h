#ifndef ESEP_LIB_LOGGER_H
#define ESEP_LIB_LOGGER_H

#include "lib/utils.h"
#include "lib/singleton.h"
#include "system.h"

#include "lib/log/standard.h"

#define MXT_LOG_IMPL(section,severity,msg) \
	::esep::lib::Logger::instance().log( \
	::esep::System::instance().elapsed(), \
	std::this_thread::get_id(), \
	section, \
	severity, \
	msg, \
	::esep::log::Source(__FILE__,__LINE__))

#define MXT_LOG_1(msg) MXT_LOG_IMPL(::esep::log::Section::SYSTEM,::esep::log::Severity::INFO,msg)
#define MXT_LOG_2(msg,section) MXT_LOG_IMPL(section,::esep::log::Severity::INFO,msg)
#define MXT_LOG_3(msg,section,severity) MXT_LOG_IMPL(section,severity,msg)

#define MXT_LOG_WHICH(...) MXT_GETA4(__VA_ARGS__,MXT_LOG_3,MXT_LOG_2,MXT_LOG_1)

#define MXT_LOG(...) MXT_LOG_WHICH(__VA_ARGS__)(__VA_ARGS__)

namespace esep
{
	namespace lib
	{
		typedef SingletonHolder<log::Standard> Logger;
	}
}

#endif
