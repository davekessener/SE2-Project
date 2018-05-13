#ifndef ESEP_LIB_LOGGER_H
#define ESEP_LIB_LOGGER_H

#include "lib/utils.h"
#include "lib/singleton.h"
#include "lib/timer.h"

#include "lib/log/types.h"
#include "lib/log/base.h"


#define MXT_LOG_IMPL(severity,msg) \
	::esep::lib::Logger::instance().log( \
		::esep::lib::Timer::instance().elapsed(), \
		std::this_thread::get_id(), \
		severity, \
		msg, \
		::esep::log::Source(__FILE__,__LINE__))

#define MXT_LOG(...)       MXT_LOG_IMPL(::esep::log::Severity::INFO,    ::esep::lib::stringify(__VA_ARGS__))
#define MXT_LOG_INFO(...)  MXT_LOG_IMPL(::esep::log::Severity::INFO,    ::esep::lib::stringify(__VA_ARGS__))
#define MXT_LOG_WARN(...)  MXT_LOG_IMPL(::esep::log::Severity::WARNING, ::esep::lib::stringify(__VA_ARGS__))
#define MXT_LOG_ERROR(...) MXT_LOG_IMPL(::esep::log::Severity::ERROR,   ::esep::lib::stringify(__VA_ARGS__))
#define MXT_LOG_FATAL(...) MXT_LOG_IMPL(::esep::log::Severity::FATAL,   ::esep::lib::stringify(__VA_ARGS__))

/**
 * Catches any std::exception or std::string
 * Use as fail-safe i.e. in threads
 */
#define MXT_CATCH_STRAY \
	catch(const std::exception& e) \
	{ \
		MXT_LOG_WARN(lib::stringify("Caught a stray exception: ", e.what())); \
	} \
	catch(const std::string& e) \
	{ \
		MXT_LOG_WARN(lib::stringify("Caught a stray string: ", e)); \
	}

/**
 * Fail-safe that catches everything
 */
#define MXT_CATCH_ALL_STRAY \
	MXT_CATCH_STRAY \
	catch(...) \
	{ \
		MXT_LOG_WARN("Caught an unknown exception!"); \
	}


#define MXT_LOCK_2(l,m) \
		l lock; \
		do { try { \
			lock = l(m); \
		} catch(const std::system_error& e) { \
			MXT_LOG_FATAL("Encountered system_error while locking mutex!"); \
			throw; \
		} } while(0)
#define MXT_LOCK_1(m) MXT_LOCK_2(lock_t,m)
#define MXT_LOCK_0() MXT_LOCK_1(mMutex)
#define MXT_LOCK_GET(...) MXT_GETA3(__VA_ARGS__,MXT_LOCK_2,MXT_LOCK_1,MXT_LOCK_0)
#define MXT_LOCK(...) MXT_LOCK_GET(__VA_ARGS__)(__VA_ARGS__)

#define MXT_SYNCHRONIZE MXT_LOCK_0()

namespace esep
{
	namespace log
	{
		template
		<
			typename ThreadingPolicy = tml::policy::SingleThreaded
		>
		class Impl : private tml::policy::ThreadingInheritor<Impl<ThreadingPolicy>, ThreadingPolicy>
		{
			typedef tml::policy::ThreadingInheritor<Impl<ThreadingPolicy>, ThreadingPolicy> Super;
			typedef Base::tid_t tid_t;
			using typename Super::Lock;

			public:
				void log(uint time, tid_t thread, Severity s, const std::string& msg, const Source& src)
					{ Lock lock(*this); mLogger.log(time, thread, s, msg, src); }
				void addSink(Writer_ptr p, Severity s)
					{ Lock lock(*this); mLogger.addSink(std::move(p), s); }

			private:
				Base mLogger;
		};
	}

	namespace lib
	{
		typedef Singleton<log::Impl<tml::policy::InstanceSynchronization<tml::policy::MultiThreaded<>>>, -3> Logger;
	}
}

#endif
