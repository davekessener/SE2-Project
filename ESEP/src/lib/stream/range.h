#ifndef ESEP_LIB_STREAM_RANGE_H
#define ESEP_LIB_STREAM_RANGE_H

namespace esep
{
	namespace lib
	{
		namespace stream
		{
			template<typename T>
			class Range : public Iterator<T>
			{
				typedef Range<T> Self;
				typedef typename Iterator<T>::Iterator_ptr Iterator_ptr;

				public:
					Range(T min, T max, T step = 1) : mMin(min), mMax(max), mStep(step) { }

					T next( ) override { T e(mMin); mMin += mStep; return e; }
					bool hasNext( ) const override { return mMin < mMax; }

					Iterator_ptr clone( ) const override { return Iterator_ptr(new Self(*this)); };

				private:
					T mMin;
					const T mMax, mStep;
			};
		}
	}
}

#endif
