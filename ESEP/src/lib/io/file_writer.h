#ifndef ESEP_IO_FILEWRITER_H
#define ESEP_IO_FILEWRITER_H

#include <fstream>

#include "lib/writer.h"

namespace esep
{
	namespace lib
	{
		class FileWriter : public Writer
		{
			public:
				FileWriter(const std::string& path) : mStream(path) { }
				void writeLine(const std::string& s) override { mStream << s << std::endl; }
			private:
				std::ofstream mStream;
		};
	}
}

#endif
