#ifndef ESEP_IO_CONTAINERREADER_H
#define ESEP_IO_CONTAINERREADER_H

#include "lib/reader.h"
#include "lib/member_wrapper.h"

namespace esep
{
	namespace lib
	{
		template
		<
			typename C
		>
		class ContainerReader : public Reader
		{
			typedef std::function<std::string(C&)> remove_fn;

			public:
				template<typename T>
				ContainerReader(T&& c, remove_fn remove = [](C& c) { std::string s(c.front()); c.pop_front(); return s; })
					: mContainer(std::forward<T>(c)), mRemove(remove) { }
				bool empty( ) const override { return mContainer.empty(); }

			protected:
				std::string doReadLine( ) override
				{
					return mRemove(mContainer);
				}

			private:
				C mContainer;
				remove_fn mRemove;
		};
	}
}

#endif
