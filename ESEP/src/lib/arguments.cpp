#include <algorithm>

#include "lib/arguments.h"

namespace esep { namespace lib {

void Arguments::process(const std::string& arg)
{
	std::string s(arg);

	if(s.size() < 3 || s[0] != '-' || s[1] != '-')
	{
		MXT_THROW_EX(InvalidArgumentException);
	}

	std::transform(s.begin(), s.end(), s.begin(), ::tolower);

	const char *i1 = s.c_str(), *i2 = i1 + s.size();

	i1 += 2;

	const char *r = i1;

	while(*r && *r != '=') ++r;

	mArgs.emplace(std::make_pair(std::string{i1, r}, std::string{*r ? r + 1 : r, i2}));
}

}}
