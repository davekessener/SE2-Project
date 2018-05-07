#ifndef ESEP_LIB_WRITER_H
#define ESEP_LIB_WRITER_H

#include <string>

namespace esep
{
	namespace lib
	{
		class Writer
		{
			public:
				virtual ~Writer( ) { }
				virtual void write(const std::string&) = 0;
			private:
		};
	}
}

#endif
