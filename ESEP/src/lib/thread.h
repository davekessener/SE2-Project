#ifndef ESEP_LIB_THREAD_H
#define ESEP_LIB_THREAD_H

#include <thread>
#include <utility>

namespace esep
{
	namespace lib
	{
		class Thread
		{
			public:
				Thread( );
				template<typename F, typename ... A>
					Thread(F&& f, A&& ... a)
						: mThread(f, std::forward<A>(a)...) { }
				Thread(Thread&&);
				~Thread( );
				Thread& operator=(Thread&&);
				template<typename F, typename ... A>
					void construct(F&& f, A&& ... a)
						{ *this = Thread(std::forward<F>(f), std::forward<A>(a)...); }
			private:
				std::thread mThread;

			private:
				Thread(const Thread&) = delete;
				Thread& operator=(const Thread&) = delete;
		};
	}
}

#endif
