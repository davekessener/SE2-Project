#ifndef ESEP_BASE_ERROR_RESETACK_H
#define ESEP_BASE_ERROR_RESETACK_H

#include "base/error/recoverable.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class ResetAck : public Recoverable
			{
				public:
					ResetAck(communication::IRecipient *b) : Recoverable(b) { }

					void enter( ) override;
					void handle(Event) override;
			};
		}
	}
}

#endif
