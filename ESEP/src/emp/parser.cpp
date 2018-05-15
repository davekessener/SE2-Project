#include "emp/parser.h"

namespace esep { namespace emp {

namespace
{
	inline constexpr char to_lower(const char c) { return (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c; }
	inline constexpr uint to_int(const char c) { return (c >= '0' && c <= '9') ? (c - '0') : (c - 'a' + 10); }
}

uint32_t Parser::parseTime(const std::string& s)
{
	return parseInt(s.c_str(), 10);
}

Location Parser::parseSensor(const std::string& s)
{
	std::string name(s);

	for(auto& c : name)
	{
		c = to_lower(c);
	}

	return Location::byName(name);
}

uint16_t Parser::parseValue(const std::string& raw)
{
	std::string s = raw;

	for(auto& c : s)
	{
		c = to_lower(c);
	}

	if(s.size() == 16)
	{
		return parseInt(s.c_str(), 2, 0xFFFF);
	}
	else if(s == "true" || s == "t" || s == "1")
	{
		return 1;
	}
	else if(s == "false" || s == "f" || s == "0")
	{
		return 0;
	}
	else
	{
		const char *p = s.c_str();

		switch(*p++)
		{
		case '0':
			if(*p++ != 'x')
			{
				MXT_THROW_EX(ParseException);
			}
		case '$':
			return parseInt(p, 16, 0xFFFF);

		default:
			return parseInt(s.c_str(), 10, 0xFFFF);
		}
	}
}

uint32_t Parser::parseInt(const char *s, uint b, uint32_t max)
{
	uint32_t v = 0;

	if(!*s)
	{
		MXT_THROW_EX(ParseException);
	}

	for(; *s ; ++s)
	{
		char c = to_lower(*s);

		if(!((c >= 'a' && c <= 'f') || (c >= '0' && c <= '9')))
		{
			MXT_THROW_EX(ParseException);
		}

		uint t = to_int(c);

		if(t >= b)
		{
			MXT_THROW_EX(ParseException);
		}

		v = v * b + t;
	}

	if(max && v > max)
	{
		MXT_THROW_EX(ParseException);
	}

	return v;
}

}}
