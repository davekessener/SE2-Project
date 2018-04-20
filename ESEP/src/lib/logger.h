#ifndef ESEP_LIB_LOGGER_H
#define ESEP_LIB_LOGGER_H

#include "lib/utils.h"
#include "lib/singleton.h"
#include "lib/timer.h"

#include "lib/log/threaded.h"

#define MXT_LOG_IMPL(section,severity,msg) \
	::esep::lib::Logger::instance().log( \
	::esep::lib::Timer::instance().elapsed(), \
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

#define MXT_CATCH_STRAY \
	catch(const std::exception& e) \
	{ \
		MXT_LOG(lib::stringify("Caught a stray exception: ", e.what())); \
	} \
	catch(const std::string& e) \
	{ \
		MXT_LOG(lib::stringify("Caught a stray string: ", e)); \
	}

#define MXT_CATCH_ALL_STRAY \
	MXT_CATCH_STRAY \
	catch(...) \
	{ \
		MXT_LOG("Caught an unknown exception!"); \
	}

namespace esep
{
	namespace lib
	{
		typedef SingletonHolder<log::Threaded> Logger;
	}
}

#endif
