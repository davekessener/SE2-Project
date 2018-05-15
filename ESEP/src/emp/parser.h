#ifndef ESEP_EMP_PARSER_H
#define ESEP_EMP_PARSER_H

#include "emp/location.h"

namespace esep
{
	namespace emp
	{
		class Parser
		{
			public:
				MXT_DEFINE_E(ParseException);

				static uint32_t parseTime(const std::string&);
				static Location parseSensor(const std::string&);
				static uint16_t parseValue(const std::string&);

			private:
				static uint32_t parseInt(const char *, uint, uint32_t = 0);

			private:
				Parser( ) = delete;
				~Parser( ) = delete;
				Parser(const Parser&) = delete;
				Parser& operator=(const Parser&) = delete;
		};
	}
}

#endif
