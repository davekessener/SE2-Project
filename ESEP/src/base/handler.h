#ifndef SRC_BASE_HANDLER_H
#define SRC_BASE_HANDLER_H

#include <memory>
#include <atomic>

#include "base/IManager.h"
#include "base/error_manager.h"
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
			typedef communication::Message Message;
			typedef communication::Packet Packet;
			typedef communication::Packet_ptr Packet_ptr;
			typedef sync::Container<Packet_ptr> container_t;
			typedef Packet::msg_t msg_t;
			typedef Packet::Location Location;
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
				Handler(ConfigObject *); //requires the IRecipient of the higher comm. layer
				~Handler();
				void setMaster(communication::IRecipient *p) { mMaster = p; }
				void handle(Event); //for event listening registration
				void accept(Packet_ptr);
				bool running( ) const { return mRunning.load(); }

			private:
				void switchManager(Message::Base);
				void handleError(Message::Error, Packet_ptr);
				void handleHAL(Event);

			private:
				communication::IRecipient * mMaster;
				ConfigObject * const mConfigData;
				std::atomic<bool> mRunning;
				qnx::Connection mConnection;
				std::unique_ptr<IManager> mConfigManager,  mRunManager, mIdleManager, mValidManager, mReadyManager;
				ErrorManager::Error_ptr mErrorManager;
				IManager* mCurrentManager;
				container_t mPacketBuffer;
				lib::Thread mHandlerThread;
		};
	}
}

#endif
