#ifndef ESEP_LOG_BASE_H
#define ESEP_LOG_BASE_H

#include <string>
#include <map>
#include <vector>

#include "lib/utils.h"
#include "lib/log/format.h"
#include "lib/thread.h"

namespace esep
{
	namespace log
	{
		enum class Section : uint
		{
			SYSTEM
		};

		enum class Severity : uint
		{
			INFO,
			WARNING,
			ERROR,
			CRITICAL
		};

		struct Source
		{
			Source(const std::string& f, int l) : file(f), line(l) { }

			const std::string file;
			const int line;
		};

		class Base
		{
			public:
			enum class Filter : uint
			{
				THIS = 1,
				SUBSECTIONS = 2,
				SUPERSECTIONS = 4
			};

			enum class EchoPolicy
			{
				EXCLUDE,
				INCLUDE
			};

			typedef std::thread::id tid_t;

			typedef format::Collection<
				uint,
				tid_t,
				Section,
				Severity,
				std::string,
				Source
			> formatters_t;

			public:
				Base( );
				virtual ~Base( ) { }
				void log(uint, tid_t, Section, Severity, const std::string&, const Source&);
				void setFormat(const std::string&);
				void setThreshold(Severity s) { mThreshold = static_cast<uint>(s); }
				void addFilter(EchoPolicy ep, Filter f, Section s) { mFilters[s] |= static_cast<uint>(f); }
				void clearFilters( ) { mFilters.clear(); }
				void setPolicy(EchoPolicy ep) { mPolicy = ep; }
				template<typename T>
					void setFormatter(T&& f)
						{ mFormatters.set(f); }
			protected:
				virtual void doWrite(const std::string&) = 0;
				virtual void doEcho(const std::string&);
			private:
				uint mThreshold;
				EchoPolicy mPolicy;
				std::map<Section, uint> mFilters;
				formatters_t mFormatters;
				std::vector<std::string> mLiterals;
				std::vector<std::pair<int, uint>> mFormat;
		};
	}
}

#endif
