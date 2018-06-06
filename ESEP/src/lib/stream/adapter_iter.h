#ifndef ESEP_LIB_STREAM_ADAPTERITER_H
#define ESEP_LIB_STREAM_ADAPTERITER_H

#include <functional>

#include "lib/stream/iterator.h"

#include "lib/tml.h"

namespace esep
{
	namespace lib
	{
		namespace stream
		{
			template
			<
				typename T1,
				typename T2
			>
			class AdapterIterator : public Iterator<tml::DoDecay<T1>>
			{
				typedef AdapterIterator<T1, T2> Self;
				typedef tml::DoDecay<T1> value_type;
				typedef tml::DoDecay<T2> other_type;
				typedef std::shared_ptr<Iterator<other_type>> Other_ptr;
				typedef std::function<T1(T2)> adapter_fn;
				typedef typename Iterator<value_type>::Iterator_ptr Iterator_ptr;

				public:
					AdapterIterator(Other_ptr i, adapter_fn f) : mI(i), mF(std::move(f)) { }
					AdapterIterator(const Self& i) : mI(i.mI->clone()), mF(i.mF) { }

					value_type next( ) override { return mF(mI->next()); }
					bool hasNext( ) const override { return mI->hasNext(); }

					Iterator_ptr clone( ) const override { return Iterator_ptr(new Self(*this)); }

				private:
					Other_ptr mI;
					adapter_fn mF;
			};
		}
	}
}

#endif
