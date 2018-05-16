#ifndef ESEP_TML_ENUM_H
#define ESEP_TML_ENUM_H

#include "lib/utils.h"

#include "lib/tml/base.h"
#include "lib/tml/type_list.h"
#include "lib/tml/logic.h"
#include "lib/tml/if.h"
#include "lib/tml/algorithms.h"

namespace esep
{
	namespace tml
	{
		namespace enum_impl
		{
			template<size_t> struct SizeTraits { };
			template<> struct SizeTraits<1> { typedef uint8_t  small_t; typedef uint16_t large_t; };
			template<> struct SizeTraits<2> { typedef uint16_t small_t; typedef uint32_t large_t; };
			template<> struct SizeTraits<4> { typedef uint32_t small_t; typedef uint64_t large_t; };
		}

		template
		<
			typename L
		>
		class CompoundEnum
		{
			typedef L enums_t;
			typedef enum_impl::SizeTraits<list::Max<list::DoApply<enums_t, Fun2Type<SizeOf>>>::Value> traits_t;
			typedef typename traits_t::large_t value_type;
			typedef typename traits_t::small_t part_type;

			template<typename T>
			using enable_if = EnableIf<list::Contains<enums_t, T>>;

			public:
			MXT_DEFINE_E(InvalidCastException);

			public:
				template<typename T, typename = enable_if<T>>
				CompoundEnum(const T& v)
					: mValue(calculate(v))
				{
				}

				template<typename T, typename = enable_if<T>>
				CompoundEnum<L>& operator=(const T& v)
				{
					mValue = calculate(v);

					return *this;
				}

				template<typename T, typename = enable_if<T>>
				bool operator==(const T& v) const
				{
					return mValue == calculate(v);
				}

				bool operator==(const CompoundEnum<L>& e) const
				{
					return mValue == e.mValue;
				}

				template<typename T>
				bool operator!=(const T& v) const
				{
					return !(*this == v);
				}

				template<typename T, typename = enable_if<T>>
				T as( ) const
				{
					if(get_header<T>() != get_header())
					{
						MXT_THROW_EX(InvalidCastException);
					}

					return static_cast<T>(get_body());
				}

				template<typename T, typename = enable_if<T>>
				bool is( ) const
				{
					return get_header<T>() == get_header();
				}

				template<typename T, typename = tml::EnableIf<tml::logic::GreaterThanOrEqualTo<tml::SizeOf<T>, tml::SizeOf<value_type>>>>
				explicit operator T( ) const
				{
					return mValue;
				}

			private:
				template<typename T>
				static constexpr value_type calculate(const T& v)
				{
					return (get_header<T>() << (sizeof(part_type) * 8)) | get_body(v);
				}

				template<typename T>
				static constexpr value_type get_header( )
				{
					return list::IndexOf<enums_t, T>::Value;
				}

				template<typename T>
				static constexpr value_type get_body(const T& v)
				{
					return static_cast<part_type>(v);
				}

				part_type get_header( ) const { return mValue >> (sizeof(part_type) * 8); }
				part_type get_body( ) const { return mValue & static_cast<part_type>(-1); }

			private:
				value_type mValue;
		};

		template<typename ... T>
		using MakeCompoundEnum = CompoundEnum<MakeTypeList<T...>>;
	}
}

#endif
