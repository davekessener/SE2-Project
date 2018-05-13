#ifndef ESEP_TML_STRING_H
#define ESEP_TML_STRING_H

#include <string>
#include <sstream>

#include "lib/tml/base.h"
#include "lib/tml/logic.h"
#include "lib/tml/type_list.h"

namespace esep
{
	namespace tml
	{
		namespace string
		{
			template<typename T>
			struct CanPrint
			{
				template<typename TT>
				using Apply = decltype(std::declval<std::ostream&>() << std::declval<const TT&>());

				static constexpr bool Value = CanCall<CanPrint<T>, T>::Value;
			};

			namespace impl
			{
				inline void stringify(std::stringstream&) { }

				template<typename T, typename ... A>
				void stringify(std::stringstream& ss, T&& o, A&& ... a)
				{
					ss << o;

					stringify(ss, std::forward<A>(a)...);
				}
			}

			template<typename ... A>
			std::string stringify(A&& ... a)
			{
				std::stringstream ss;

				impl::stringify(ss, std::forward<A>(a)...);

				return ss.str();
			}
		}
	}
}

template<typename T1, typename T2,
	typename = esep::tml::EnableIf<esep::tml::logic::And<esep::tml::string::CanPrint<T1>, esep::tml::string::CanPrint<T2>>>>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& p)
{
	return os << "std::pair<" << p.first << ", " << p.second << ">";
}

template<typename T, typename = esep::tml::EnableIf<
	esep::tml::logic::And<
		esep::tml::IsExplicitlyCastable<T, int>,
		esep::tml::Not<esep::tml::IsImplicitlyCastable<T, int>>
	>
>>
std::ostream& operator<<(std::ostream& os, T&& o)
{
	return os << static_cast<int>(std::forward<T>(o));
}

#endif
