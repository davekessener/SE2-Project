#include <memory>
#include <iostream>

#include "test/ft/serial/watchdog.h"

#include "serial/actual_connection.h"
#include "serial/bsp_client.h"
#include "serial/watchdog.h"

#define MXT_SERIAL_DEVICE "/dev/ser4"

namespace esep { namespace test { namespace functional {


typedef std::unique_ptr<serial::Connection> connection_ptr;
typedef std::unique_ptr<serial::Client> client_ptr;
typedef serial::Client::buffer_t buffer_t;
typedef serial::Connection::ConnectionClosedException ConnectionClosedException;

// Helper functions that convert between std::string and Client::buffer_t
inline buffer_t buffer(const std::string& s) { return    buffer_t(s.cbegin(), s.cend()); }
inline std::string str(const buffer_t& b)    { return std::string(b.cbegin(), b.cend()); }


void testWatchdog(void)
{
	lib::Thread readThread; // Read thread has to be constructed BEFORE the client, so it gets destroyed LAST

	connection_ptr con(new serial::ActualConnection(MXT_SERIAL_DEVICE));
	client_ptr     bsp(new serial::BSPClient(std::move(con)));
	client_ptr  client(new serial::Watchdog(std::move(bsp)));

	std::atomic<bool> mRunning;


	mRunning = true;

	std::cout << "SERIAL TEST - START\n"
			  << "Enter 's Message' to send and 'q' to quit." << std::endl;


	// Reader thread that reads from the connection and writes to the console
	readThread.construct([&client, &mRunning](void) {
		try
		{
			while(mRunning.load())
			{
				auto p = client->read();

				std::cout << "Read message: '" << str(p) << "'!" << std::endl;
			}
		}
		catch(const serial::Connection::ConnectionClosedException& e)
		{
			std::cout << "Connection was closed.\nPress [ENTER] to quit!" << std::endl;
			mRunning = false;
		}
		MXT_CATCH_ALL_STRAY
	});


	while(mRunning.load())
	{
		std::string line;
		std::getline(std::cin, line);


		if(!mRunning.load()) break; // The reading thread has detected a disconnect while we where busy waiting for input


		if(line.empty()) // No command given, try again
		{
			continue;
		}
		else if(line[0] == 's') // Sending a message
		{
			if(line.size() < 2)
			{
				continue;
			}
			else
			{
				// The message is the entire input line starting with the 3rd character
				// i.e. the line  's Hello, World!'  sends  'Hello, World!'  to the remote host
				std::string msg = line.substr(2);

				client->write(buffer(msg));

				std::cout << "Wrote message '" << msg << "' (" << msg.size() << ")!" << std::endl;
			}
		}
		else if(line[0] == 'q')
		{
			mRunning = false;
		}
	}
}

}}}
