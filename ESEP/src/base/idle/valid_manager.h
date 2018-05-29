#ifndef ESEP_BASE_VALIDMANAGER_H
#define ESEP_BASE_VALIDMANAGER_H

#include "base/idle/base.h"

namespace esep
{
	namespace base
	{
		class ValidManager : public IdleBase
		{
			public:
				ValidManager(communication::IRecipient *c) : mCom(c) { }

			protected:
				void shortPress( ) override
					{ mCom->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Master::RUN)); }
				void longPress( ) override
					{ mCom->accept(std::make_shared<Packet>(Location::BASE, Location::MASTER, Message::Master::CONFIG)); }

			private:
				communication::IRecipient * const mCom;
		};
	}
}

#endif
