#ifndef ESEP_MASTER_PLUGIN_SORTABLE_H
#define ESEP_MASTER_PLUGIN_SORTABLE_H

#include "master/plugin.h"

namespace esep
{
	namespace master
	{
		namespace plugin
		{
			namespace impl
			{
				typedef Plugin::Type Type;
				typedef tml::Nil Nil;

				MXT_DEFINE_E(ArrayOutOfBoundsException);

				template<Type H, typename T>
				struct Cons
				{
					static constexpr Type Car = H;
					typedef T Cdr;
				};

				template<Type ... T>
				struct List;

				template<Type T1, Type ... T>
				struct List<T1, T...>
				{
					typedef Cons<T1, typename List<T...>::Type> Type;
				};

				template<>
				struct List<>
				{
					typedef Nil Type;
				};

				template<Type ... T>
				using MakeList = typename List<T...>::Type;

				template<typename L>
				struct Includes;

				template<Type T, typename L>
				struct Includes<Cons<T, L>>
				{
					static bool eval(Type t)
					{
						return t == T || Includes<L>::eval(t);
					}
				};

				template<>
				struct Includes<Nil>
				{
					static bool eval(Type)
					{
						return false;
					}
				};

				template<typename L>
				struct Length;

				template<Type T, typename L>
				struct Length<Cons<T, L>>
				{
					static constexpr size_t Value = 1 + Length<L>::Value;
				};

				template<>
				struct Length<Nil>
				{
					static constexpr size_t Value = 0;
				};

				template<typename L1, typename L2>
				struct Append;

				template<Type T, typename R, typename L>
				struct Append<Cons<T, R>, L>
				{
					typedef Cons<T, typename Append<R, L>::Type> Type;
				};

				template<typename L>
				struct Append<Nil, L>
				{
					typedef L Type;
				};

				template<typename L>
				struct Get;

				template<Type T, typename R>
				struct Get<Cons<T, R>>
				{
					static Type eval(size_t i)
					{
						return i ? Get<R>::eval(i - 1) : T;
					}
				};

				template<>
				struct Get<Nil>
				{
					static Type eval(size_t)
					{
						MXT_THROW_EX(ArrayOutOfBoundsException);
					}
				};

				template<size_t I, typename L>
				struct Skip;

				template<size_t I, Type T, typename R>
				struct Skip<I, Cons<T, R>> : Skip<I - 1, R>
				{
				};

				template<Type T, typename R>
				struct Skip<0, Cons<T, R>>
				{
					typedef Cons<T, R> Type;
				};

				template<>
				struct Skip<0, Nil>
				{
					typedef Nil Type;
				};

				template<typename L>
				struct StartsWith;

				template<Type T, typename R>
				struct StartsWith<Cons<T, R>>
				{
					template<typename C>
					static bool eval(const C& c, size_t i = 0)
					{
						return (i > c.size()) || (c.at(i) == T && StartsWith<R>::eval(c, i + 1));
					}
				};

				template<>
				struct StartsWith<Nil>
				{
					template<typename C>
					static bool eval(const C&, size_t = 0)
					{
						return false;
					}
				};

				template<typename L>
				struct Find
				{
					typedef typename Append<L, L>::Type double_t;
					static constexpr size_t LENGTH = Length<L>::Value;

					template<typename C1, typename C2, size_t I = LENGTH>
					static void eval(C1& result, const C2& arg)
					{
						typedef typename Skip<LENGTH - I, double_t>::Type list_t;

						if(I)
						{
							if(StartsWith<list_t>::eval(arg))
							{
								result.push_back(Get<list_t>::eval(arg.size()));
							}

							eval<C1, C2, (I > 0 ? I - 1 : 0)>(result, arg);
						}
					}
				};
			}

			template<Plugin::Type ... T>
			class Sortable : public virtual Plugin
			{
				typedef impl::MakeList<T...> types_t;
				static constexpr size_t SIZE = impl::Length<types_t>::Value;

				public:
				MXT_DEFINE_E(InvalidTypeException);
				MXT_DEFINE_E(InvalidStateException);

				public:
					Sortable(Type t) : Plugin(t)
					{
						if(!contained(t))
						{
							MXT_THROW_EX(InvalidTypeException);
						}
					}

					Action decide(const history_t& h) override
					{
						return type() == predictNext(h) ? Action::KEEP : Action::TOSS_S;
					}

				private:
					static bool contained(Type t) { return impl::Includes<types_t>::eval(t); }

					Type predictNext(const history_t& h) const
					{
						std::deque<Type> possibilities;
						std::deque<Type> history;

						for(const auto& t : h)
						{
							if(contained(t)) history.push_front(t);
							if(history.size() == SIZE) break;
						}

						if(history.empty())
						{
							return type();
						}

						impl::Find<types_t>::eval(possibilities, history);

						if(possibilities.empty())
						{
							MXT_THROW_EX(InvalidStateException);
						}
						else if(std::find(possibilities.begin(), possibilities.end(), type()) != possibilities.end())
						{
							return type();
						}
						else
						{
							return possibilities.front();
						}
					}
			};
		}
	}
}

#endif
