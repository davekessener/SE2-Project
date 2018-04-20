#ifndef ESEP_SERIAL_VOIDCONNECTION_H
#define ESEP_SERIAL_VOIDCONNECTION_H

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "serial/connection.h"

namespace esep
{
	namespace serial
	{
		class VoidConnection : public Connection
		{
			typedef std::unique_lock<std::mutex> lock_t;

			public:
				VoidConnection( ) { mOpen = true; }
				virtual ~VoidConnection( ) { close(); }
				virtual void write(const byte_t *, size_t);
				virtual void read(byte_t *, size_t);
				virtual void close( );
				virtual void open(const std::string&) { mOpen = true; }
				virtual bool isOpen( ) { return mOpen.load(); }
			private:
				std::atomic<bool> mOpen;
				std::mutex mMutex;
				std::condition_variable mCond;
		};
	}
}

#endif
