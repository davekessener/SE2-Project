#ifndef ESEP_LIB_UTILS_H
#define ESEP_LIB_UTILS_H

#include <stdint.h>

#include <ios>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <mutex>

#include "lib/tml.h"

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
			template<typename T>
			struct CanBeWrittenToStream
			{
				template<typename TT>
				using impl = decltype(std::declval<std::ostream&>() << std::declval<TT>());

				static constexpr bool Value = tml::CanApply<impl, T>::Value;
			};

			template<typename T>
			struct StreamGenerator
			{
				static constexpr bool Value = true;

				static std::string convert(const T& o)
				{
					std::stringstream ss;

					ss << o;

					return ss.str();
				}
			};

			template<typename T, typename TT>
			struct CastGenerator
			{
				static constexpr bool Value = true;

				static std::string convert(const T& o)
				{
					return StreamGenerator<TT>::convert(static_cast<TT>(o));
				}
			};

			template<typename T>
			struct FailedGenerator
			{
				static constexpr bool Value = false;
			};

			template<typename T>
			struct StringGenerator
				: tml::DoIf<
				  	  CanBeWrittenToStream<T>,
					  tml::Type2Type<StreamGenerator<T>>,
					  tml::If<
					  	  tml::CanCastTo<T, int>,
						  tml::Type2Type<CastGenerator<T, int>>,
						  tml::Type2Type<FailedGenerator<T>>
					  >
				  >
			{
			};

			template<typename T>
			tml::EnableIf<impl::StringGenerator<T>, std::string> to_string(T&& o)
			{
				return impl::StringGenerator<T>::convert(std::forward<T>(o));
			}

			template<typename T1, typename T2>
			std::string to_string(const std::pair<T1, T2>& o)
			{
				std::stringstream ss;

				ss << "std::pair<" << to_string(o.first) << ", " << to_string(o.second) << ">";

				return ss.str();
			}
		}

		namespace impl
		{
			static_assert(!CanBeWrittenToStream<std::pair<int, int>>::Value, "Can't write pairs to stream!");
			static_assert(tml::Not<CanBeWrittenToStream<std::pair<int, int>>>::Value, "Can't write pairs to stream!");

			template<typename S>
			void stringifyImpl(S&&) { }

			template<typename S, typename T, typename ... TT>
			void stringifyImpl(S&& ss, T&& o, TT&& ... tt)
			{
				ss << to_string(std::forward<T>(o));

				stringifyImpl(std::forward<S>(ss), std::forward<TT>(tt)...);
			}
		}

		template<typename ... A>
		std::string stringify(A&& ... a)
		{
			std::stringstream ss;

			impl::stringifyImpl(ss, std::forward<A>(a)...);

			return ss.str();
		}

		template<typename T, typename F>
		auto with_temporary(T o, F&& f) -> decltype(f(o))
		{
			return f(o);
		}
	}
}

#endif

