#ifndef SRC_BASE_ERROR_MANAGER_H
#define SRC_BASE_ERROR_MANAGER_H

#include <memory>

#include "base/error/base.h"

namespace esep
{
	namespace base
	{
		class ErrorManager : public IManager
		{
			public:
			typedef std::unique_ptr<error::Base> Error_ptr;

			public:
				MXT_DEFINE_E(IncorrectMessageException);

			public:
				static Error_ptr create(communication::IRecipient *, communication::Packet_ptr);
		};
	}
}

#endif
