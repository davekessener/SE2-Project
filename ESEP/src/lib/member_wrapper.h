#ifndef ESEP_LIB_MEMBERWRAPPER_H
#define ESEP_LIB_MEMBERWRAPPER_H

#include <utility>

namespace esep
{
	namespace lib
	{
		template<typename R, typename C, typename ... A>
		struct MemberWrapper
		{
			typedef R (C::*cb_fn)(A...);

			MemberWrapper(cb_fn f) : mF(f) { }

			template<typename T>
			R operator()(T&& c, A&& ... a)
			{
				return (static_cast<C&>(c).*mF)(std::forward<A>(a)...);
			}

		private:
			cb_fn mF;
		};

		template<typename R, typename C, typename ... A>
		struct ConstMemberWrapper
		{
			typedef R (C::*cb_fn)(A...) const;

			ConstMemberWrapper(cb_fn f) : mF(f) { }

			template<typename T>
			R operator()(T&& c, A&& ... a)
			{
				return (static_cast<C&>(c).*mF)(std::forward<A>(a)...);
			}

		private:
			cb_fn mF;
		};
	}
}

#endif
