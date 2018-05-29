#ifndef SRC_BASE_IDLE_MANAGER_H
#define SRC_BASE_IDLE_MANAGER_H

#include <memory>

#include "base/config_object.h"

#include "base/idle/base.h"

#include "lib/timer.h"

namespace esep
{
	namespace base
	{
		class IdleManager : public IdleBase
		{
			public:
				IdleManager(communication::IRecipient *com, ConfigObject *c) : mCom(com), mConfig(c) { }

				void enter( ) override;
				void leave( ) override;

			protected:
				void shortPress( ) override;
				void longPress( ) override
					{ mCom->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Master::CONFIG)); }

			private:
				communication::IRecipient * const mCom;
				ConfigObject * const mConfig;
		};
	}
}

#endif
