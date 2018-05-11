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
			typename C,
			typename A = ConstMemberWrapper<const std::string&, C>,
			typename R = MemberWrapper<void, C>
		>
		class ContainerReader : public Reader
		{
			public:
				template<typename T>
				ContainerReader(T&& c, A access = &C::front, R remove = &C::pop_front)
					: mContainer(std::forward<T>(c)), mAccess(access), mRemove(remove) { }
				bool empty( ) const override { return mContainer.empty(); }

			protected:
				std::string doReadLine( ) override
				{
					std::string s(mAccess(mContainer));

					mRemove(mContainer);

					return s;
				}

			private:
				C mContainer;
				A mAccess;
				R mRemove;
		};
	}
}

#endif
