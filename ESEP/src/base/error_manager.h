
#ifndef SRC_BASE_ERROR_MANAGER_H_
#define SRC_BASE_ERROR_MANAGER_H_

#include <memory>

#include "base/IManager.h"

namespace esep
{
	namespace base
	{
		class ErrorManager : public IManager
		{
			public:
				MXT_DEFINE_E(IncorrectMessageException);

			public:
				static std::unique_ptr<IManager> create(communication::IRecipient *, communication::Packet_ptr);
		};
}
}

#endif
