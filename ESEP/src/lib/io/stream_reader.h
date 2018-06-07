#ifndef ESEP_IO_STREAMREADER_H
#define ESEP_IO_STREAMREADER_H

#include <iostream>

#include "lib/reader.h"

namespace esep
{
	namespace lib
	{
		class StreamReader : public Reader
		{
			public:
				StreamReader(std::istream& in) : mIn(&in) { }

				bool empty( ) const override { return false; }

			protected:
				std::string doReadLine( ) { std::string line; while(!std::getline(*mIn, line)); return line; }

			private:
				std::istream * const mIn;
		};
	}
}

#endif
