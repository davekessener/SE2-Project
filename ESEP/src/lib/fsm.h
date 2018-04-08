#ifndef ESEP_LIB_FSM_H
#define ESEP_LIB_FSM_H

namespace esep
{
	namespace lib
	{
		namespace fsm
		{
			class State
			{
				public:
					virtual ~State( ) { }
					virtual void enter( ) { }
					virtual void exit( ) { }
			};

			template<typename T>
			class FSM
			{
				public:
				typedef T event_type;

				typedef std::function<void(State&, State&, const event_type&)> transition_fn;
				typedef std::pair<State *, event_type> key_type;
				typedef std::pair<State *, transition_fn> value_type;
				typedef std::map<key_type, value_type> trans_table_t;
				typedef std::vector<State *> state_table_t;

				public:
				private:
					FSM(state_table_t&& s, trans_table_t&& t) : mStates(std::move(s)), mTransitions(std::move(t))
						{ mCurrent = mStates.first(); }
					void process(const event_type& e)
					{
						auto i = mTransitions.find(std::make_pair(mCurrent, e));

						if(i != mTransitions.end())
						{
							State *oldState = mCurrent;
							State *newState = i->second.first;
							transition_fn *f = &i->second.second;

							bool change = oldState != new State;

							if(change)
							{
								oldState->exit();
							}

							mCurrent = newState;
							(*f)(*oldState, *newState, e);

							if(change)
							{
								newState->enter();
							}
						}
					}
				private:
					state_table_t mStates;
					trans_table_t mTransitions;
					State *mCurrent;

					template<typename TT>
					friend class Generator<TT>;
			};

			template<typename T>
			class Generator
			{
				typedef FSM<T>::transition_fn transition_fn;
				typedef FSM<T>::trans_table_t trans_table_t;
				typedef FSM<T>::state_table_t state_table_t;

				public:
					template<typename S, typename ... SS>
						void addStates(S&& o, SS&& ... s)
					{
						mStates.push_back(static_cast<tml::DoDecay<S> *>(o));
						addStates(s...);
					}
					Generator<T>& addTransition(State *a, State *b, const T& e, transition_fn f = transition_fn())
					{
						mTransitions[std::make_pair(a, e)] = std::make_pair(b, f);

						return *this;
					}
					FSM<T> generate(void)
					{
						return FSM<T>(std::move(mStates), std::move(mTransitions));
					}
				private:
					void addStates(void) { }
				private:
					state_table_t mStates;
					trans_table_t mTransitions;
			};
		}
	}
}

#endif
