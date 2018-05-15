#ifndef SRC_BASE_HANDLER_H
#define SRC_BASE_HANDLER_H

#include <memory>
#include <atomic>

#include "base/IManager.h"
#include "base/config_object.h"

#include "communication/IRecipient.h"

#include "lib/thread.h"
#include "lib/sync/container.h"


namespace esep
{
	namespace base
	{
		class Handler : public communication::IRecipient
		{
			private:
			typedef sync::Container<communication::Packet_ptr> container_t;
			typedef communication::Packet Packet;
			typedef Packet::Location Location;
			typedef Packet::Message Message;
			typedef hal::HAL::Event Event;

			enum class MessageType : int8_t
			{
				STOP_RUNNING,
				HAL_EVENT,
				PACKET_IN_BUFFER,
			};

			public:
			MXT_DEFINE_E(UndefinedMasterException);

			public:
				Handler( ); //requires the IRecipient of the higher comm. layer
				~Handler();
				void setMaster(communication::IRecipient *p) { mMaster = p; }
				void handle(hal::HAL::Event); //for event listening registration
				void accept(std::shared_ptr<communication::Packet>);
				bool running( ) const { return mRunning.load(); }

			private:
				void switchManager(IManager *);

			private:
				communication::IRecipient* mMaster;
				lib::Thread mHandlerThread;
				std::atomic<bool> mRunning;
				qnx::Connection mConnection;
				ConfigObject mConfigData;
				std::shared_ptr<IManager> mConfigManager,  mRunManager, mIdleManager, mReadyManager, mErrorManager;
				IManager* mCurrentManager;
				container_t mPacketBuffer;
		};
	}
}

#endif /* SRC_BASE_HANDLER_H */
