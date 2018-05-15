#ifndef ESEP_IO_STREAMWRITER_H
#define ESEP_IO_STREAMWRITER_H

#include <iostream>

#include "lib/writer.h"

namespace esep
{
	namespace lib
	{
		class StreamWriter : public Writer
		{
			public:
				StreamWriter(std::ostream& os) : mOS(&os) { }
				void writeLine(const std::string& s) override { (*mOS) << s << std::endl; }
			private:
				std::ostream * const mOS;
		};
	}
}

#endif
