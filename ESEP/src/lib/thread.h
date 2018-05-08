#ifndef ESEP_LIB_THREAD_H
#define ESEP_LIB_THREAD_H

#ifndef NO_QNX
#	include <thread>
#	include <mutex>
#	include <condition_variable>
#else
#	include <mingw.thread.h>
#	include <mingw.mutex.h>
#	include <mingw.condition_variable.h>
#endif

#include <utility>

namespace esep
{
	namespace lib
	{
		/**
		 * Wrapper for std::thread
		 *
		 * Provides automatic join() call in destructor.
		 */
		class Thread
		{
			public:
				Thread( );
				template<typename F, typename ... A>
					Thread(F&& f, A&& ... a)
						: mThread(f, std::forward<A>(a)...) { logCreation(); }
				Thread(Thread&&);
				~Thread( );
				Thread& operator=(Thread&&);
				template<typename F, typename ... A>
					void construct(F&& f, A&& ... a)
						{ *this = Thread(std::forward<F>(f), std::forward<A>(a)...); logCreation(); }
				void join( );
				bool active( ) const;
			private:
				void logCreation( );

			private:
				std::thread mThread;

			private:
				Thread(const Thread&) = delete;
				Thread& operator=(const Thread&) = delete;
		};
	}
}

#endif
