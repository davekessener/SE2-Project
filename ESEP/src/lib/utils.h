#ifndef ESEP_LIB_UTILS_H
#define ESEP_LIB_UTILS_H

#include <stdint.h>

#include <ios>
#include <iomanip>
#include <string>
#include <sstream>

typedef unsigned uint;

namespace esep
{
	namespace lib
	{
		template<uint B, typename T>
		std::string hex(const T& v)
		{
			std::stringstream ss;

			ss << std::hex << std::setfill('0') << std::setw(B / 4) << v;

			return ss.str();
		}

		namespace impl
		{
			template<typename S>
			void stringifyImpl(S&) { }

			template<typename S, typename T, typename ... TT>
			void stringifyImpl(S& ss, const T& o, const TT& ... tt)
			{
				ss << o;
				stringifyImpl(ss, tt...);
			}
		}

		template<typename ... T>
		std::string stringify(const T& ... o)
		{
			std::stringstream ss;

			impl::stringifyImpl(ss, o...);

			return ss.str();
		}
	}
}

#endif

