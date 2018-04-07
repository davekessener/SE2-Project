#include <iostream>
#include <functional>

#include "lib/log/base.h"

#define MXT_DEFAULT_FORMAT "%8time% [%4severity% in %6section% (%24source%)] (%thread%)| %message%"

namespace esep { namespace log {

// # ===========================================================================================

namespace
{
	struct SectionImpl
	{
		uint32_t impl[1];

		SectionImpl(void)
		{
			impl[static_cast<uint>(Section::SYSTEM)] = 0;
		}

		bool matches(Section s, Section r, uint f)
		{
			bool v = false;
			std::function<bool(uint, uint)> isChild;

			isChild = [this, &isChild](uint st, uint rt) {
				if(impl[rt] & st) return true;

				for(uint i = 0 ; i < sizeof(uint32_t) * 8 ; ++i)
				{
					if((impl[rt] & (1 << i)) && isChild(st, i))
						return true;
				}

				return false;
			};


			if(f & static_cast<uint>(Base::Filter::THIS))
			{
				v = (s == r);
			}

			if(!v && (f & static_cast<uint>(Base::Filter::SUBSECTIONS)))
			{
				v = isChild(1 << (static_cast<uint>(s) - 1), static_cast<uint>(r));
			}

			if(!v && (f & static_cast<uint>(Base::Filter::SUPERSECTIONS)))
			{
				v = isChild(1 << (static_cast<uint>(r) - 1), static_cast<uint>(s));
			}

			return v;
		}
	};

	bool shouldEcho(Section s, const std::map<Section, uint>& f)
	{
		static SectionImpl sectionManager;

		for(const auto& p : f)
		{
			if(sectionManager.matches(s, p.first, p.second))
			{
				return true;
			}
		}

		return false;
	}
}

// # ============================================================================================

const char * const SECTIONS[] = {
	"SYSTEM"
};

const char * const SEVERITIES[] = {
	"INFO",
	"WARN",
	"ERR",
	"CRIT"
};

const char * const FORMAT_ARGS[] = {
	"time",
	"thread",
	"section",
	"severity",
	"message",
	"source"
};

// # ============================================================================================

Base::Base(void)
	: mThreshold(static_cast<uint>(Severity::WARNING))
	, mPolicy(EchoPolicy::EXCLUDE)
{
	setFormat(MXT_DEFAULT_FORMAT);
	setFormatter([](uint t)               -> std::string { return lib::stringify(t); });
	setFormatter([](tid_t tid)            -> std::string { return "0"; });
	setFormatter([](Section s)            -> std::string { return SECTIONS[static_cast<uint>(s)]; });
	setFormatter([](Severity s)           -> std::string { return SEVERITIES[static_cast<uint>(s)]; });
	setFormatter([](const Source& s)      -> std::string { return lib::stringify(s.file, ": ", s.line); });
	setFormatter([](const std::string& s) -> std::string { return s; });
}

void Base::setFormat(const std::string& f)
{
	format::Parser parser(FORMAT_ARGS);

	for(const char& c : f)
	{
		parser.parse(c);
	}

	parser.parse('\0');

	std::swap(mLiterals, parser.literals);
	std::swap(mFormat, parser.format);
}

void Base::log(uint time, tid_t tid, Section section, Severity severity, const std::string& message, const Source& source)
{
	std::stringstream ss;

	auto i = mLiterals.cbegin();

	for(const auto& p : mFormat)
	{
		if(p.first == -1)
		{
			ss << *i++;
		}
		else
		{
			ss << std::setw(p.second) << mFormatters.format(p.first, time, tid, section, severity, message, source);
		}
	}

	std::string s(ss.str());

	doWrite(s);

	if(static_cast<uint>(severity) >= mThreshold
		|| (mPolicy == EchoPolicy::INCLUDE &&  shouldEcho(section, mFilters))
		|| (mPolicy == EchoPolicy::EXCLUDE && !shouldEcho(section, mFilters)))
	{
		doEcho(s);
	}
}

void Base::doEcho(const std::string& msg)
{
	std::cout << msg << std::endl;
}

}}
