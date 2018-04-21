#ifndef ESEP_LIB_TUPLE_H
#define ESEP_LIB_TUPLE_H

#include "lib/utils.h"
#include "lib/tml.h"

namespace esep
{
	namespace lib
	{
		/**
		 * Tuple template
		 *
		 * Stores a variable amount of object of different types.
		 *
		 * Example:
		 * 		// objects of type my_tuple_t hold two ints, a double and a string
		 * 		typedef lib::MakeTuple<int, double, std::string, int> my_tuple_t;
		 *
		 * 		my_tuple_t tuple_object; // uses default constructor for its elements
		 * 		my_tuple_t other_tuple(7, 3.141, "Sample Text", 42); // constructs tuple by invoking the copy-constructor for its elements
		 *
		 * 		// members of a tuple can be accessed by both its type as well as its position
		 * 		// within the defining list of types.
		 * 		// if a tuple has multiple members with the same type they can only be accessed
		 * 		// with their index
		 * 		tuple_object.get<double>() = 2.76; // tuple_object's double member now holds 2.76
		 * 		other_tuple.get<0>() *= 2; // other_tuple's first int member now holds 14
		 *
		 * 		std::cout << other_tuple.get<2>() << std::endl; // prints 'Sample Text' to cout
		 */

		namespace tuple
		{
			using tml::TypeList;
			using tml::Nil;

			template<typename T, size_t I>
			struct ObjectHolder
			{
				public:
					ObjectHolder( ) { }
					ObjectHolder(T&& o) : mObject(std::move(o)) { }
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
						: TupleImpl<T, I + 1>(std::forward<TT>(a)...)
						, ObjectHolder<H, I>(std::move(o))
					{ }
				template<typename ... TT>
					TupleImpl(const H& o, TT&& ... a)
						: TupleImpl<T, I + 1>(std::forward<TT>(a)...)
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
			typedef L value_types;

		    template<typename ... TT>
		        Tuple(TT&& ... a)
		            : tuple::TupleImpl<L, 0>(std::forward<TT>(a)...)
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
		    return Tuple<tml::DoApply<tml::MakeTypeList<T...>, tml::Decay>>(std::forward<T>(a)...);
		}

		template<typename ... T>
		using MakeTuple = Tuple<tml::MakeTypeList<T...>>;
	}
}

#endif
