#ifndef ESEP_COMMUNICATION_BASE_H
#define ESEP_COMMUNICATION_BASE_H

#include <memory>
#include <atomic>

#include "lib/utils.h"
#include "lib/thread.h"
#include "lib/sync/container.h"

#include "serial/client.h"

#include "communication/IRecipient.h"
#include "communication/packet.h"

namespace esep
{
	namespace communication
	{
		class Base : public IRecipient
		{
			typedef std::unique_ptr<serial::Client> Client_ptr;
			typedef sync::Container<Packet_ptr> container_t;

			enum class Messages : int8_t
			{
				SHUTDOWN,
				PACKET
			};

			public:
			MXT_DEFINE_E(MissingBaseException);
			MXT_DEFINE_E(BaseOverrideException);

			public:
				Base(Client_ptr);
				virtual ~Base( );

				void setBase(IRecipient *);
				void accept(Packet_ptr) override;
				bool busy( ) const { return !mBuffer.empty(); }

			protected:
				IRecipient& base( );
				void send(Packet_ptr);
				void shutdown( );

				virtual void process(Packet_ptr) = 0;

			private:
				std::atomic<bool> mRunning, mShuttingDown;
				IRecipient * mBase;
				lib::Thread mProcessingThread, mSerialThread;
				Client_ptr mSerial;
				container_t mBuffer;
		};
	}
}

#endif
