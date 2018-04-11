#ifndef ESEP_LOG_STANDARD_H
#define ESEP_LOG_STANDARD_H

#include <memory>
#include <ios>

#include "lib/log/base.h"

namespace esep
{
	namespace log
	{
		class Standard : public Base
		{
			public:
				Standard( );
			protected:
				virtual void doWrite(const std::string&);
			private:
				std::shared_ptr<std::ostream> mOS;
		};
	}
}

#endif
