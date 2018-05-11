#ifndef ESEP_LIB_WRITER_H
#define ESEP_LIB_WRITER_H

#include <string>
#include <memory>

namespace esep
{
	namespace lib
	{
		class Writer
		{
			public:
				virtual ~Writer( ) { }
				virtual void writeLine(const std::string&) = 0;
			private:
		};

		typedef std::unique_ptr<Writer> Writer_ptr;
	}
}

#endif
