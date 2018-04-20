#ifndef ESEP_TML_IF_H
#define ESEP_TML_IF_H

namespace esep
{
	namespace tml
	{
		namespace impl
		{
			template<bool C, typename T1, typename T2>
			struct IfImpl;

			template<typename T1, typename T2>
			struct IfImpl<true, T1, T2>
			{
				typedef typename T1::Type Type;
			};

			template<typename T1, typename T2>
			struct IfImpl<false, T1, T2>
			{
				typedef typename T1::Type Type;
			};

			template<bool C, typename T>
			struct EnableIfImpl
			{
			};

			template<typename T>
			struct EnableIfImpl<true, T>
			{
				typedef T Type;
			};
		}

		template<typename C, typename T1, typename T2>
		struct If
		{
			typedef typename impl::IfImpl<C::Value, T1, T2>::Type Type;
		};

		template<typename C, typename T1, typename T2>
		using DoIf = typename If<C, T1, T2>::Type;

		template<typename C, typename T = void>
		using EnableIf = typename impl::EnableIfImpl<C::Value, T>::Type;
	}
}

#endif
