#ifndef ESEP_IO_FILEREADER_H
#define ESEP_IO_FILEREADER_H

#include <fstream>
#include <deque>

#include "lib/reader.h"

namespace esep
{
	namespace lib
	{
		class LazyFileReader : public Reader
		{
			public:
				LazyFileReader(const std::string&);
				bool empty( ) const override { return mEmpty; }

			protected:
				std::string doReadLine( ) override;
				void nextLine( );

			private:
				std::string mNextLine;
				std::ifstream mInput;
				bool mEmpty;
		};

		class ImmediateFileReader : public Reader
		{
			public:
				ImmediateFileReader(const std::string&);
				bool empty( ) const override { return mBuffer.empty(); }

			protected:
				std::string doReadLine( ) override;

			private:
				std::deque<std::string> mBuffer;
		};
	}
}

#endif
