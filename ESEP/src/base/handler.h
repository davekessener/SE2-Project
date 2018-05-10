#ifndef SRC_BASE_HANDLER_H
#define SRC_BASE_HANDLER_H

#include <memory>
#include <atomic>

#include "base/IManager.h"
#include "communication/IRecipient.h"
#include "config_manager.h"
#include "config_object.h"
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
			typedef communication::Packet::Message Message;

			enum class MessageType : int8_t
			{
				STOP_RUNNING,
				HAL_EVENT,
				PACKET_IN_BUFFER,
			};

			public:
				Handler(communication::IRecipient *); //requires the IRecipient of the higher comm. layer
				~Handler();
				void handle(hal::HAL::Event); //for event listening registration
				void accept(std::shared_ptr<communication::Packet>);

			private:
				void switchManager(IManager *);

			private:
				communication::IRecipient* mCommunicationModul;
				lib::Thread mHandlerThread;
				std::atomic<bool> mRunning;
				qnx::Connection mConnection;
				ConfigObject mConfigData;
				std::shared_ptr<IManager> mConfigManager,  mRunManager, mErrorManager, mDefaultManager;
				IManager* mCurrentManager;
				container_t mPacketBuffer;
		};
	}
}

#endif /* SRC_BASE_HANDLER_H */
