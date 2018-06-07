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

		template<typename L>
		class Tuple
		{
			template<typename T, size_t I>
			struct Holder
			{
				public:
				typedef T value_type;

				public:
					Holder( ) { }

					template<typename TT>
						Holder(TT&& o)
							: mObj(std::forward<TT>(o)) { }

					value_type& get( ) { return mObj; }
					const value_type& get( ) const { return mObj; }

				private:
					value_type mObj;
			};

			template<typename ... T>
			struct Helper : T...
			{
				Helper( ) { }

				template<typename ... TT>
				Helper(TT&& ... a) : T(std::forward<TT>(a))...
				{
				}
			};

			struct HolderAdapter
			{
				template<typename T, typename I>
				struct Apply
				{
					typedef Holder<T, I::Value> Type;
				};
			};

			typedef L value_types;
			typedef typename tml::list::ToVariadic<
				tml::Fun2Type<Helper>,
				tml::list::DoApplyWithIndex<
					value_types,
					HolderAdapter
				>
			>::Type container_t;

			public:
				Tuple( ) { }

				template<typename ... T>
				Tuple(T&& ... a)
					: mContainer(std::forward<T>(a)...)
				{
				}

				template<typename T, typename = tml::EnableIf<tml::list::Contains<value_types, T>>>
				T& get( )
				{
					typedef Holder<T, tml::list::IndexOf<value_types, T>::Value> t_t;

					return mContainer.t_t::get();
				}

				template<typename T, typename = tml::EnableIf<tml::list::Contains<value_types, T>>>
				const T& get( ) const
				{
					typedef Holder<T, tml::list::IndexOf<value_types, T>::Value> t_t;

					return mContainer.t_t::get();
				}

				template<size_t I, typename = tml::EnableIf<tml::list::IsValidIndex<value_types, I>>>
				tml::list::DoGet<value_types, I>& get( )
				{
					typedef Holder<tml::list::DoGet<value_types, I>, I> t_t;

					return mContainer.t_t::get();
				}


				template<size_t I, typename = tml::EnableIf<tml::list::IsValidIndex<value_types, I>>>
				const tml::list::DoGet<value_types, I>& get( ) const
				{
					typedef Holder<tml::list::DoGet<value_types, I>, I> t_t;

					return mContainer.t_t::get();
				}

			private:
				container_t mContainer;
		};

		template<typename ... T>
		using MakeTuple = Tuple<tml::MakeTypeList<T...>>;
	}
}

#endif
