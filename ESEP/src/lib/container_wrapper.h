#ifndef ESEP_LIB_CONTAINERWRAPPER_H
#define ESEP_LIB_CONTAINERWRAPPER_H

#include "lib/tml.h"

namespace esep
{
	namespace lib
	{
		template<typename T>
		class ContainerWrapper
		{
			public:
			typedef tml::DoDecay<T> container_type;
			typedef typename container_type::const_iterator iterator;

			public:
				ContainerWrapper(const container_type& o)
					: mObj(o)
						{ }
				iterator begin( ) const { return mObj.cbegin(); }
				iterator end( ) const { return mObj.cend(); }
				size_t size( ) const { return mObj.size(); }

			private:
				const container_type& mObj;
		};

		template<typename T, size_t N>
		class ContainerWrapper<T(&)[N]>
		{
			public:
			typedef const T *iterator;

			public:
				ContainerWrapper(const T *p)
					: mP(p)
						{ }
				iterator begin( ) const { return mP; }
				iterator end( ) const { return mP + N; }
				size_t size( ) const { return N; }

			private:
				const T * const mP;
		};

		template<typename T>
		class ContainerWrapper<T&&>
		{
			public:
			typedef tml::DoDecay<T> container_type;
			typedef typename container_type::const_iterator iterator;

			public:
				ContainerWrapper(container_type&& c)
					: mObj(std::move(c))
						{ }
				iterator begin( ) const { return mObj.cbegin(); }
				iterator end( ) const { return mObj.cend(); }
				size_t size( ) const { return mObj.size(); }

			private:
				const container_type mObj;
		};

		template<typename T>
		auto wrap_container(T&& c) -> ContainerWrapper<decltype(c)>
		{
			return ContainerWrapper<decltype(c)>(std::forward<T>(c));
		}
	}
}

#endif
