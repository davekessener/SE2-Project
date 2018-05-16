#ifndef ESEP_BASE_ERROR_BASE_H
#define ESEP_BASE_ERROR_BASE_H

#include "base/IManager.h"

namespace esep
{
	namespace base
	{
		namespace error
		{
			class Base : public IManager
			{
				public:
					virtual int priority( ) const = 0;

					void enter( ) override;
			};
		}
	}
}

#endif
