#ifndef ESEP_LIB_UTILS_H
#define ESEP_LIB_UTILS_H

#include <stdint.h>

#include <ios>
#include <iomanip>
#include <string>
#include <sstream>

#define MXT_GETA1(a1,...) a1
#define MXT_GETA2(a1,a2,...) a2
#define MXT_GETA3(a1,a2,a3,...) a3
#define MXT_GETA4(a1,a2,a3,a4,...) a4
#define MXT_GETA5(a1,a2,a3,a4,a5,...) a5

#define MXT_THROW_STR(...) ::esep::lib::stringify(__VA_ARGS__," [",__FILE__,": ",__LINE__,"]")

#define MXT_THROW(...) throw MXT_THROW_STR(__VA_ARGS__)
#define MXT_THROW_E(klass,...) throw klass(MXT_THROW_STR(__VA_ARGS__))

typedef unsigned uint;
typedef uint8_t byte_t;

namespace esep
{
	namespace lib
	{
		template<uint B, typename T>
		std::string hex(const T& v)
		{
			std::stringstream ss;

			ss << std::hex << std::setfill('0') << std::setw(B / 4) << static_cast<uint64_t>(v);

			return ss.str();
		}

		namespace impl
		{
			template<typename S>
			void stringifyImpl(S&) { }

			template<typename S, typename T1, typename T2>
			S& operator<<(S& s, const std::pair<T1, T2>& e)
			{
				s << "std::pair<" << e.first << "," << e.second << ">";

				return s;
			}

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

