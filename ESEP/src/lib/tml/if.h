#ifndef ESEP_TML_IF_H
#define ESEP_TML_IF_H

namespace esep
{
	namespace tml
	{
		namespace if_impl
		{
			template<bool V, typename T1, typename T2>
			struct Impl;

			template<typename T1, typename T2>
			struct Impl<true, T1, T2> : T1
			{
			};

			template<typename T1, typename T2>
			struct Impl<false, T1, T2> : T2
			{
			};

			template<bool V, typename T>
			struct Enable
			{
			};

			template<typename T>
			struct Enable<true, T>
			{
				typedef T Type;
			};

			template<bool V, typename T>
			struct Inherit
			{
			};

			template<typename T>
			struct Inherit<true, T> : T::Type
			{
			};
		}

		template<typename C, typename T1, typename T2>
		struct If : if_impl::Impl<C::Value, T1, T2>
		{
		};

		template<typename C, typename T1, typename T2>
		using DoIf = typename If<C, T1, T2>::Type;

		template<typename C, typename T = void>
		using EnableIf = typename if_impl::Enable<C::Value, T>::Type;

		template<typename C, typename T>
		struct InheritIf : if_impl::Inherit<C::Value, T>
		{
		};
	}
}

#endif
