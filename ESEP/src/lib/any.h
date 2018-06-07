#ifndef ESEP_LIB_ANY_H
#define ESEP_LIB_ANY_H

#include <memory>

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		class Any
		{
			struct Base { };

			template<typename T>
			struct Holder : Base
			{
				template<typename ... TT>
				Holder(TT&& ... o) : object(std::forward<TT>(o)...) { }

				T object;
			};

			public:
			MXT_DEFINE_E(NoObjectException);
			MXT_DEFINE_E(InvalidTypeException);

			public:
				Any( ) { }
				template<typename T>
					Any(T&& o) : mObj(new Holder<tml::DoDecay<T>>(std::forward<T>(o))) { }
				Any(const Any& o) : mObj(o.mObj) { }
				Any(Any&& o) : mObj(std::move(o.mObj)) { }

				template<typename T>
				Any& operator=(T&& o)
				{
					mObj.reset(new Holder<tml::DoDecay<T>>(std::forward<T>(o)));

					return *this;
				}

				Any& operator=(const Any& o)
				{
					mObj = o.mObj;

					return *this;
				}

				Any& operator=(Any&& o)
				{
					mObj = std::move(o.mObj);

					return *this;
				}

				template<typename T, typename ... TT>
				void emplace(TT&& ... a)
				{
					mObj.reset(new Holder<T>(std::forward<TT>(a)...));
				}

				bool empty( ) const { return !static_cast<bool>(*this); }
				explicit operator bool( ) const { return static_cast<bool>(mObj); }

				template<typename T>
				T& get( )
				{
					typedef Holder<tml::DoDecay<T>> holder_t;

					if(empty())
					{
						MXT_THROW_EX(NoObjectException);
					}

					if(holder_t *p = dynamic_cast<holder_t *>(mObj.get()))
					{
						return p->object;
					}
					else
					{
						MXT_THROW_EX(InvalidTypeException);
					}
				}

			private:
				std::shared_ptr<Base> mObj;
		};
	}
}

#endif
