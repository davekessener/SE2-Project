#include <iostream>
#include <functional>

#include "lib/log/base.h"

#include "lib/io/stream_writer.h"

#define MXT_DEFAULT_FORMAT "%80message% | %8time% (%thread%) [%5severity% in (%source%)]"

namespace esep { namespace log {

// # ===========================================================================================

const char * const SEVERITIES[] = {
	"INFO",
	"WARN",
	"ERROR",
	"FATAL"
};

const char * const FORMAT_ARGS[] = {
	"time",
	"thread",
	"severity",
	"message",
	"source"
};

constexpr uint N_SEVERITIES = sizeof(SEVERITIES)  / sizeof(*SEVERITIES);
constexpr uint N_FORMATS    = sizeof(FORMAT_ARGS) / sizeof(*FORMAT_ARGS);

// # ============================================================================================

Base::Base(void)
{
	setFormat(MXT_DEFAULT_FORMAT);
	setFormatter([](uint t)               -> std::string { return lib::stringify(t); });
	setFormatter([](tid_t tid)            -> std::string { return lib::stringify(tid); });
	setFormatter([](Severity s)           -> std::string { return SEVERITIES[static_cast<uint>(s)]; });
	setFormatter([](const Source& s)      -> std::string { return lib::stringify(s.file, ": ", s.line); });
	setFormatter([](const std::string& s) -> std::string { return s; });
}

void Base::setFormat(const std::string& f)
{
	format::Parser parser(FORMAT_ARGS);

	parser.parseAll(f);

	std::swap(mLiterals, parser.literals);
	std::swap(mFormat, parser.format);
}

void Base::log(uint time, tid_t tid, Severity severity, const std::string& msg, const Source& source)
{
	std::stringstream ss;
	std::string message(msg);

	std::transform(message.begin(), message.end(), message.begin(), [](const char& c) { return c == '\n' ? ' ' : c; });

	auto i1 = mLiterals.cbegin(), i2 = mLiterals.cend();

	for(const auto& p : mFormat)
	{
		if(p.first == -1)
		{
			if(i1 == i2)
				MXT_THROW("Malformed logger format string!");

			ss << *i1++;
		}
		else
		{
			ss << std::left << std::setw(p.second) << mFormatters.format(p.first, time, tid, severity, message, source);
		}
	}

	std::string s(ss.str());

	for(auto& sink : mSinks)
	{
		if(sink.threshold <= static_cast<uint>(severity))
		{
			sink.writer->writeLine(s);
		}
	}
}

}}
