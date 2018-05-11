#include "lib/io/file_reader.h"

namespace esep { namespace lib {

LazyFileReader::LazyFileReader(const std::string& fn)
	: mInput(fn)
{
	nextLine();
}

std::string LazyFileReader::doReadLine(void)
{
	std::string r = mNextLine;

	nextLine();

	return r;
}

void LazyFileReader::nextLine(void)
{
	if((mEmpty = !std::getline(mInput, mNextLine)))
	{
		mInput.close();
	}
}

ImmediateFileReader::ImmediateFileReader(const std::string& fn)
{
	std::ifstream in(fn);

	for(std::string line ; std::getline(in, line) ;)
	{
		mBuffer.push_back(line);
	}
}

std::string ImmediateFileReader::doReadLine(void)
{
	std::string s(mBuffer.front());

	mBuffer.pop_front();

	return s;
}

}}
