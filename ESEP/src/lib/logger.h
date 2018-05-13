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
#define MXT_LOG_2(msg,severity) MXT_LOG_IMPL(::esep::log::Section::SYSTEM,severity,msg)
#define MXT_LOG_3(msg,severity,section) MXT_LOG_IMPL(section,severity,msg)

#define MXT_LOG_WHICH(...) MXT_GETA4(__VA_ARGS__,MXT_LOG_3,MXT_LOG_2,MXT_LOG_1)

/**
 * The MXT_LOG macro calls the log() function of the logger singleton.
 *
 * The macro's C++ style signature is
 * void MXT_LOG(const std::string& message, Severity = Severity::INFO, Section = Section::SYSTEM);
 */
#define MXT_LOG(...) MXT_LOG_WHICH(__VA_ARGS__)(__VA_ARGS__)


/**
 * Catches any std::exception or std::string
 * Use as fail-safe i.e. in threads
 */
#define MXT_CATCH_STRAY \
	catch(const std::exception& e) \
	{ \
		MXT_LOG(lib::stringify("Caught a stray exception: ", e.what())); \
	} \
	catch(const std::string& e) \
	{ \
		MXT_LOG(lib::stringify("Caught a stray string: ", e)); \
	}

/**
 * Fail-safe that catches everything
 */
#define MXT_CATCH_ALL_STRAY \
	MXT_CATCH_STRAY \
	catch(...) \
	{ \
		MXT_LOG("Caught an unknown exception!"); \
	}


#define MXT_LOCK_2(l,m) \
		l lock; \
		do { try { \
			lock = l(m); \
		} catch(const std::system_error& e) { \
			MXT_LOG("Encountered system_error while locking mutex!"); \
			throw; \
		} } while(0)
#define MXT_LOCK_1(m) MXT_LOCK_2(lock_t,m)
#define MXT_LOCK_0() MXT_LOCK_1(mMutex)
#define MXT_LOCK_GET(...) MXT_GETA3(__VA_ARGS__,MXT_LOCK_2,MXT_LOCK_1,MXT_LOCK_0)
#define MXT_LOCK(...) MXT_LOCK_GET(__VA_ARGS__)(__VA_ARGS__)

#define MXT_SYNCHRONIZE MXT_LOCK_0()

namespace esep
{
	namespace lib
	{
		typedef Singleton<log::Threaded, -3> Logger;
	}
}

#endif
