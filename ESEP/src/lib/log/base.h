#ifndef ESEP_LOG_BASE_H
#define ESEP_LOG_BASE_H

#include <string>
#include <map>
#include <vector>
#include <forward_list>
#include <memory>

#include "lib/utils.h"
#include "lib/thread.h"
#include "lib/writer.h"

#include "lib/log/types.h"
#include "lib/log/format.h"

namespace esep
{
	namespace log
	{
		class Base
		{
			public:
			typedef std::thread::id tid_t;

			typedef format::Collection<
				uint,
				tid_t,
				Severity,
				std::string,
				Source
			> formatters_t;

			public:
				Base( );

				void log(uint, tid_t, Severity, const std::string&, const Source&);
				void addSink(Writer_ptr p, Severity s) { mSinks.emplace_front(std::move(p), s); }
				void setFormat(const std::string&);
				template<typename T>
					void setFormatter(T&& f)
						{ mFormatters.set(f); }

			private:
				formatters_t mFormatters;
				std::forward_list<Sink> mSinks;
				std::vector<std::string> mLiterals;
				std::vector<std::pair<int, uint>> mFormat;
		};
	}
}

#endif
