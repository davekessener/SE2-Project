#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "lib/io/network_writer.h"

#include "lib/logger.h"

namespace esep { namespace lib {

class NetworkWriter::Impl
{
	public:
		Impl(const std::string&, uint);
		~Impl( );
		void write(const std::string&);
	private:
		uint mSocket;
};

// # --------------------------------------------------------------------------------------

NetworkWriter::NetworkWriter(const std::string& addr, uint port)
	: pImpl(new Impl(addr, port))
{
}

NetworkWriter::~NetworkWriter(void)
{
	delete pImpl;
}

void NetworkWriter::write(const std::string& msg)
{
	pImpl->write(msg);
}

// # --------------------------------------------------------------------------------------

NetworkWriter::Impl::Impl(const std::string& addr, uint port)
{
	struct sockaddr_in server;

	int s = socket(AF_INET, SOCK_STREAM, 0);

	if(s < 0)
	{
		MXT_THROW_EX(ConnectionFailedException);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(addr.c_str());

	if(connect(s, (struct sockaddr *) &server, sizeof(server)) < 0)
	{
		MXT_THROW_EX(ConnectionFailedException);
	}

	mSocket = s;
}

NetworkWriter::Impl::~Impl(void)
{
}

void NetworkWriter::Impl::write(const std::string& msg)
{
	send(mSocket, msg.c_str(), msg.size(), 0);
	send(mSocket, "\n", 1, 0);
}

}}
