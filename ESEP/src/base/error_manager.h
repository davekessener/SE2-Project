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
				typedef communication::Packet_ptr Packet_ptr;
				typedef communication::IRecipient IRecipient;

			public:
				MXT_DEFINE_E(IncorrectMessageException);

			public:
				ErrorManager(communication::IRecipient *);
				void enter( ) override;
				void leave( ) override;
				void accept(Packet_ptr) override;
				void handle(Event) override;
//				static Error_ptr create(communication::IRecipient *, communication::Packet_ptr);

			private:
				IRecipient * const mHandler;
				Error_ptr mCurrentError;
		};
	}
}

#endif
