#ifndef ESEP_LOG_STANDARD_H
#define ESEP_LOG_STANDARD_H

#include <memory>
#include <ios>
#include <fstream>

#include "lib/log/base.h"

namespace esep
{
	namespace log
	{
		class Standard : public Base
		{
			typedef Base::Writer_ptr Writer_ptr;

			public:
				Standard( );
			protected:
				virtual void doWrite(const std::string&);
			private:
				std::ofstream mOS;
		};
	}
}

#endif
