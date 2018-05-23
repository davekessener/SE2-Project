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

			template<typename, typename> class Hierarchy;
			template<typename> class Machine;
			template<typename> class Leaf;

			template<typename B>
			class State : public B
			{
				typedef State<B> state_t;

				public:
					virtual ~State( ) { }

					virtual void initial(state_t *) = 0;

				protected:
					virtual void enter( ) { }
					virtual void leave( ) { }

					state_t *parent( ) { return mParent; }

					virtual state_t *current( ) = 0;
					virtual void current(state_t *) = 0;
					virtual void history( ) = 0;

				private:
					State(state_t *parent)
						: mParent(parent)
					{
					}

				private:
					state_t *mParent;

					template<typename, typename>
					friend class Hierarchy;

					friend class Machine<B>;
					friend class Leaf<B>;
			};

			template<typename B>
			class Machine : public State<B>
			{
				typedef State<B> state_t;

				public:
					Machine(bool history, state_t *parent)
						: state_t(parent)
						, mHistory(history)
						, mCurrent(nullptr)
						, mStart(nullptr)
					{
					}
					virtual ~Machine( ) { }

				protected:
					virtual state_t *current( ) override final { return mCurrent; }
					virtual void current(state_t *s) override final { mCurrent = s; }
					virtual void history( ) override final { if(!mHistory) mCurrent = mStart; }
					virtual void initial(state_t *s) override final
					{
						if(mStart)
							MXT_THROW_EX(OverrideInitialStateException);

						mCurrent = mStart = s;
					}

				private:
					bool mHistory;
					state_t *mCurrent, *mStart;
			};

			template<typename B>
			class Leaf : public State<B>
			{
				typedef State<B> state_t;

				public:
					Leaf(state_t *parent)
						: state_t(parent)
					{
						if(!parent)
							MXT_THROW_EX(SingularStateMachine);
					}
					virtual ~Leaf( ) { }

				protected:
					virtual state_t *current( ) override final { return nullptr; }
					virtual void current(state_t *) override final { }
					virtual void initial(state_t *) override final { MXT_THROW_EX(InvalidOperationException); }
					virtual void history( ) override final { }
			};

			template<typename T, typename B>
			struct Types
			{
				typedef State<B> state_t;
				typedef std::function<void(state_t&, state_t&, const T&)> transition_fn;
				typedef std::pair<state_t *, T> key_type;
				typedef std::pair<state_t *, transition_fn> value_type;
				typedef std::map<key_type, value_type> transitions_t;
				typedef std::set<state_t *> states_t;
			};

			template<typename, typename> struct Builder;

			template<typename T, typename B>
			class Hierarchy
			{
				typedef Types<T, B> types;
				typedef typename types::state_t state_t;
				typedef typename types::key_type key_type;

				public:
					Hierarchy( ) : mCurrent(nullptr) { }

					bool process(const T& e)
					{
						if(!mCurrent)
							MXT_THROW_EX(IllegalStateException);

						auto t = mTransitions.end();
						state_t *s = mCurrent;

						do
						{
							if((t = mTransitions.find(key_type(s, e))) != mTransitions.end()) break;
						}
						while((s = s->parent()));

						if(t == mTransitions.end())
						{
							return false;
						}

						s = t->second.first;
						auto f = t->second.second;
						std::function<bool(state_t *, state_t *)> is_ancestor = [&](state_t *s1, state_t *s2) -> bool
							{ return s2 == nullptr ? false : (s1 == s2) ? true : is_ancestor(s1, s2->parent()); };

						state_t *c = mCurrent;
						for(; !is_ancestor(c, s) ; c = c->parent())
						{
							if(!c)
								MXT_THROW_EX(DisjointedTreeException);

							c->history();
							c->leave();
						}

						if(static_cast<bool>(f))
						{
							f(*mCurrent, *s, e);
						}

						std::function<void(state_t *)> enter_tree = [&](state_t *a) {
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

					state_t *getActive( ) { return mCurrent; }

				private:
					Hierarchy(state_t *i, typename types::states_t&& states, typename types::transitions_t&& trans)
						: mStates(std::move(states))
						, mTransitions(std::move(trans))
					{
						if(mStates.size() == 0)
							MXT_THROW_EX(EmptyFSMException);
						if(!i)
							MXT_THROW_EX(EmptyFSMException);

						mCurrent = i;
					}

					void enterState(state_t *s)
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
					state_t *mCurrent;

					friend class Builder<T, B>;
			};

			template
			<
				typename T,
				typename B = tml::Null
			>
			class Builder
			{
				typedef Types<T, B> types;
				typedef typename types::state_t state_t;

				public:
					~Builder( )
					{
						mInitial.release();

						for(state_t *s : mStates)
						{
							delete s;
						}
					}

					template<typename ... A>
					void add(std::unique_ptr<state_t> s, A&& ... a)
					{
						mStates.insert(s.release());

						add(std::forward<A>(a)...);
					}

					void transition(state_t *s1, state_t *s2, const T& e, typename types::transition_fn f = typename types::transition_fn())
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

					void initial(state_t *s)
					{
						if(!mStates.count(s))
							MXT_THROW_EX(UnknownStateException);

						mInitial.reset(s);
					}

					Hierarchy<T, B> build( )
					{
						return Hierarchy<T, B>(mInitial.release(), std::move(mStates), std::move(mTransitions));
					}

				private:
					void add( ) { }

				private:
					typename types::states_t mStates;
					typename types::transitions_t mTransitions;
					std::unique_ptr<state_t> mInitial;
			};
		}
	}
}

#endif
