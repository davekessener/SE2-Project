#ifndef ESEP_LIB_HSM_H
#define ESEP_LIB_HSM_H

#include <map>
#include <set>
#include <vector>
#include <functional>
#include <stdexcept>
#include <memory>

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		namespace hsm
		{
			MXT_DEFINE_E(OverrideInitialStateException);
			MXT_DEFINE_E(SingularStateMachine);
			MXT_DEFINE_E(InvalidOperationException);
			MXT_DEFINE_E(EmptyFSMException);
			MXT_DEFINE_E(IllegalStateException);
			MXT_DEFINE_E(DisjointedTreeException);
			MXT_DEFINE_E(UnknownStateException);
			MXT_DEFINE_E(AmbiguousTransitionException);

			template<typename T> class Hierarchy;

			class State
			{
				public:
					virtual ~State( ) { }

					virtual void initial(State *) = 0;

				protected:
					virtual void enter( ) { }
					virtual void leave( ) { }

					State *parent( ) { return mParent; }

					virtual State *current( ) = 0;
					virtual void current(State *) = 0;
					virtual void history( ) = 0;

				private:
					State(State *parent)
						: mParent(parent)
					{
					}

				private:
					State *mParent;

					template<typename T>
					friend class Hierarchy;

					friend class Machine;
					friend class Leaf;
			};

			class Machine : public State
			{
				public:
					Machine(bool history = false, State *parent = nullptr)
						: State(parent)
						, mHistory(history)
						, mCurrent(nullptr)
						, mStart(nullptr)
					{
					}

				protected:
					virtual State *current( ) override final { return mCurrent; }
					virtual void current(State *s) override final { mCurrent = s; }
					virtual void history( ) override final { if(!mHistory) mCurrent = mStart; }
					virtual void initial(State *s) override final
					{
						if(mStart)
							MXT_THROW_EX(OverrideInitialStateException);

						mCurrent = mStart = s;
					}

				private:
					bool mHistory;
					State *mCurrent, *mStart;
			};

			class Leaf : public State
			{
				public:
					Leaf(State *parent)
						: State(parent)
					{
						if(!parent)
							MXT_THROW_EX(SingularStateMachine);
					}

				protected:
					virtual State *current( ) override final { return nullptr; }
					virtual void current(State *) override final { }
					virtual void initial(State *) override final { MXT_THROW_EX(InvalidOperationException); }
					virtual void history( ) override final { }
			};

			template<typename T>
			struct Types
			{
				typedef std::function<void(State&, State&, const T&)> transition_fn;
				typedef std::pair<State *, T> key_type;
				typedef std::pair<State *, transition_fn> value_type;
				typedef std::map<key_type, value_type> transitions_t;
				typedef std::set<State *> states_t;
			};

			template<typename T> struct Builder;

			template<typename T>
			class Hierarchy
			{
				typedef Types<T> types;

				public:
					bool process(const T& e)
					{
						if(!mCurrent)
							MXT_THROW_EX(IllegalStateException);

						auto t = mTransitions.end();
						State *s = mCurrent;

						do
						{
							if((t = mTransitions.find(typename types::key_type(s, e))) != mTransitions.end()) break;
						}
						while((s = s->parent()));

						if(t == mTransitions.end())
						{
							return false;
						}

						s = t->second.first;
						auto f = t->second.second;
						std::function<bool(State *, State *)> is_ancestor = [&](State *s1, State *s2) -> bool
							{ return s2 == nullptr ? false : (s1 == s2) ? true : is_ancestor(s1, s2->parent()); };

						State *c = mCurrent;
						for(; !is_ancestor(c, s) ; c = c->parent())
						{
							if(!c)
								MXT_THROW_EX(DisjointedTreeException);

							c->history();
							c->leave();
						}

						f(*mCurrent, *s, e);

						std::function<void(State *)> enter_tree = [&](State *a) {
							if(a != c)
							{
								enter_tree(a->parent());

								a->parent()->current(a);
								if(a != s) a->enter();
							}
						};

						enter_tree(s);
						enterState(s);

						return true;
					}

				private:
					Hierarchy(State *i, typename types::states_t&& states, typename types::transitions_t&& trans)
						: mStates(std::move(states))
						, mTransitions(std::move(trans))
					{
						if(mStates.size() == 0)
							MXT_THROW_EX(EmptyFSMException);
						if(!i)
							MXT_THROW_EX(EmptyFSMException);

						mCurrent = i;
					}

					void enterState(State *s)
					{
						do
						{
							(mCurrent = s)->enter();
						}
						while((s = s->current()));
					}

				private:
					typename types::states_t mStates;
					typename types::transitions_t mTransitions;
					State *mCurrent;

					friend class Builder<T>;
			};

			template<typename T>
			class Builder
			{
				typedef Types<T> types;

				public:
					~Builder( )
					{
						mInitial.release();

						for(State *s : mStates)
						{
							delete s;
						}
					}

					template<typename ... A>
					void add(std::unique_ptr<State> s, A&& ... a)
					{
						mStates.insert(s.release());

						add(std::forward<A>(a)...);
					}

					void transition(State *s1, State *s2, const T& e, typename types::transition_fn f = typename types::transition_fn())
					{
						if(!mStates.count(s1))
							MXT_THROW_EX(UnknownStateException);

						if(!mStates.count(s2))
							MXT_THROW_EX(UnknownStateException);

						typename types::key_type key(s1, e);
						typename types::value_type entry(s2, f);

						if(mTransitions.count(key))
							MXT_THROW_EX(AmbiguousTransitionException);

						mTransitions.emplace(std::make_pair(key, entry));
					}

					void initial(State *s)
					{
						if(!mStates.count(s))
							MXT_THROW_EX(UnknownStateException);

						mInitial.reset(s);
					}

					Hierarchy<T> build( )
					{
						return Hierarchy<T>(mInitial.release(), std::move(mStates), std::move(mTransitions));
					}

				private:
					void add( ) { }

				private:
					typename types::states_t mStates;
					typename types::transitions_t mTransitions;
					std::unique_ptr<State> mInitial;
			};
		}
	}
}

#endif
