#ifndef ESEP_LIB_TUPLE_H
#define ESEP_LIB_TUPLE_H

#include "lib/utils.h"
#include "lib/tml.h"

namespace esep
{
	namespace lib
	{
		namespace tuple
		{
			using tml::TypeList;
			using tml::Nil;

			template<typename T, size_t I>
			struct ObjectHolder
			{
				public:
					ObjectHolder( ) { }
					ObjectHolder(T&& o) : mObject(o) { }
					ObjectHolder(const T& o) : mObject(o) { }
					T& get( ) { return mObject; }
					const T& get( ) const { return mObject; }
				private:
					T mObject;
			};

			template<typename L, size_t I>
			struct TupleImpl;

			template<typename H, typename T, size_t I>
			struct TupleImpl<TypeList<H, T>, I>
				: public TupleImpl<T, I + 1>
				, protected ObjectHolder<H, I>
			{
				TupleImpl( ) { }
				template<typename ... TT>
					TupleImpl(H&& o, TT&& ... a)
						: TupleImpl<T, I + 1>(a...)
						, ObjectHolder<H, I>(o)
					{ }
				template<typename ... TT>
					TupleImpl(const H& o, TT&& ... a)
						: TupleImpl<T, I + 1>(a...)
						, ObjectHolder<H, I>(o)
					{ }
			};

			template<size_t I>
			struct TupleImpl<Nil, I>
			{
			};
		}

		template<typename L>
		struct Tuple : public tuple::TupleImpl<L, 0>
		{
		    template<typename ... TT>
		        Tuple(TT&& ... a)
		            : tuple::TupleImpl<L, 0>(a...)
		                { }

		    template<typename T>
		        T& get( )
		            { return this->tuple::ObjectHolder<T, tml::IndexOf<L, T>::Value>::get(); }

		    template<size_t I>
		        typename tml::Get<L, I>::Type get( )
		            { return this->tuple::ObjectHolder<typename tml::Get<L, I>::Type, I>::get(); }
		};

		template<typename ... T>
		Tuple<tml::DoApply<tml::MakeTypeList<T...>, tml::Decay>> make_tuple(T&& ... a)
		{
		    return Tuple<tml::DoApply<tml::MakeTypeList<T...>, tml::Decay>>(a...);
		}
	}
}

#endif
