#ifndef ESEP_SYSTEM_H
#define ESEP_SYSTEM_H

#include "lib/utils.h"
#include "lib/arguments.h"
#include "lib/singleton.h"

#include "hal/hal.h"

#include "base/config_object.h"

namespace esep
{
	namespace system
	{
		class Impl
		{
			MXT_DEFINE_E(InvalidRunArgumentException);
			MXT_DEFINE_E(ConnectionException);

			public:
				Impl( );
				~Impl( );
				void run(const lib::Arguments&);

			private:
				base::ConfigObject mConfig;
				hal::HAL *mHAL;
		};
	}

	typedef lib::Singleton<system::Impl, 0> System;
}

#endif
