#ifndef ESEP_LIB_UTILS_H
#define ESEP_LIB_UTILS_H

#include <stdint.h>

#include <ios>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>


#define MXT_GETA1(a1,...) a1
#define MXT_GETA2(a1,a2,...) a2
#define MXT_GETA3(a1,a2,a3,...) a3
#define MXT_GETA4(a1,a2,a3,a4,...) a4
#define MXT_GETA5(a1,a2,a3,a4,a5,...) a5


#define MXT_THROW_STR(...) ::esep::lib::stringify(__VA_ARGS__," [",__FILE__,": ",__LINE__,"]")


// MXT_THROW macro throws an std::string
#define MXT_THROW(...) throw MXT_THROW_STR(__VA_ARGS__)

// MXT_THROW_E macro throws an exception of a given class that is passed a string during construction
#define MXT_THROW_E(klass,...) throw klass(MXT_THROW_STR(__VA_ARGS__))

// MXT_THROW_EX macro throws an exception of a given class that is passed the current file and line during construction
#define MXT_THROW_EX(klass) throw klass(::esep::lib::stringify(__FILE__ ": ", __LINE__))

// Defines an exception deriving from std::runtime_error. Best used in conjuction with the MXT_THROW_EX macro
#define MXT_DEFINE_E(name) \
	struct name : public std::runtime_error \
	{ \
		name( ) \
			: std::runtime_error( \
					::esep::lib::stringify(#name " [", __FILE__, ": ", __LINE__, "]")) \
				{ } \
		name(const std::string& s) \
			: std::runtime_error( \
					::esep::lib::stringify("[" #name "] ", s)) \
			    { } \
	}


typedef unsigned uint;
typedef uint8_t byte_t;

namespace esep
{
	namespace lib
	{
		typedef std::vector<std::string> args_t;

		/**
		 * The lib::hex helper function accepts an argument of integral
		 * type and converts it to a hex string. The arguments width in
		 * bits is passed as template parameter.
		 *
		 * Example:
		 * 		std::cout << lib::hex<16>(100) << std::endl; // prints '0064' to the console
		 */
		template<uint B, typename T>
		std::string hex(const T& v)
		{
			std::stringstream ss;

			ss << std::hex << std::setfill('0') << std::setw(B / 4) << static_cast<uint64_t>(v);

			return ss.str();
		}

		/**
		 * The stringify helper function accepts a variable number of arguments which
		 * are all converted to strings and concatenated by running them through an
		 * std::stringstream object.
		 *
		 * Example:
		 * 		// prints 'A char has 1 byte(s).'
		 * 		std::cout << lib::stringify("A char has ", sizeof(char), " byte(s).") << std::endl;
		 */

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

