#ifndef ESEP_LOG_TYPES_H
#define ESEP_LOG_TYPES_H

#include <string>

#include "lib/utils.h"
#include "lib/writer.h"

namespace esep
{
	namespace log
	{
		typedef lib::Writer_ptr Writer_ptr;

		enum class Severity : uint
		{
			INFO,
			WARNING,
			ERROR,
			FATAL
		};

		struct Source
		{
			Source(const std::string& f, int l) : file(f), line(l) { }

			const std::string file;
			const int line;
		};

		struct Sink
		{
			Sink(Writer_ptr p, Severity t) : writer(std::move(p)), threshold(static_cast<uint>(t)) { }
			Sink(const Sink&) = delete;

			const Writer_ptr writer;
			const uint threshold;
		};
	}
}

#endif
