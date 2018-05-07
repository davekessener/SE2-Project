#ifndef ESEP_LOG_THREADED_H
#define ESEP_LOG_THREADED_H

#include <fstream>

#include "lib/thread.h"

#include "lib/log/base.h"

namespace esep
{
	namespace log
	{
		class Threaded : public Base
		{
			typedef Base::Writer_ptr Writer_ptr;
			typedef std::unique_lock<std::mutex> lock_t;

			public:
				Threaded( );
			protected:
				virtual void doWrite(const std::string&);
				virtual void doEcho(const std::string&);
			private:
				std::ofstream mOut;
				std::mutex mMutex;
		};
	}
}

#endif
