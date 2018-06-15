#ifndef ESEP_LIB_PETRINET_H
#define ESEP_LIB_PETRINET_H

#include <vector>
#include <forward_list>
#include <functional>
#include <map>

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		template<typename T>
		class PetriNet
		{
			typedef std::vector<uint> states_t;
			typedef std::function<void(void)> trans_fn;
			typedef std::pair<uint, uint> relation_t;
			typedef std::vector<relation_t> connection_t;
			typedef std::initializer_list<relation_t> rel_list;
			typedef std::tuple<T, connection_t, connection_t, trans_fn> transition_t;
			typedef std::vector<transition_t> transitions_t;
			typedef std::map<uint, trans_fn> watch_table_t;

			enum { T_TRIGGER, T_FROM, T_TO, T_F };

			public:
				PetriNet(uint s, const T& e) : mEmpty(e), mStates(s), mChanged(false) { }

				void transition(const T& e, rel_list&& from, rel_list&& to, trans_fn f = trans_fn())
				{
					mTransitions.emplace_back(e, connection_t(from.begin(), from.end()), connection_t(to.begin(), to.end()), std::move(f));
				}

				void set(uint s, uint v) { mStates.at(s) = v; }
				uint get(uint s) const { return mStates.at(s); }

				void listen(uint s, trans_fn f) { mWatchers[s] = f; }

				bool process(const T& e)
				{
					std::forward_list<transition_t *> to_fire;

					mChanged = false;

					for(auto& t : mTransitions)
					{
						if(std::get<T_TRIGGER>(t) == e && canFire(t))
						{
							to_fire.emplace_front(&t);
						}
					}

					for(auto& p : to_fire)
					{
						if(canFire(*p))
						{
							fire(*p);
						}
					}

					for(auto i1 = mTransitions.begin(), i2 = mTransitions.end() ; i1 != i2 ; ++i1)
					{
						if(std::get<T_TRIGGER>(*i1) == mEmpty && canFire(*i1))
						{
							fire(*i1);
							i1 = mTransitions.begin();
						}
					}

					return mChanged;
				}

			private:
				bool canFire(const transition_t& t) const
				{
					for(auto& r : std::get<T_FROM>(t))
					{
						if(r.second ? mStates.at(r.first) < r.second : mStates.at(r.first))
						{
							return false;
						}
					}

					return true;
				}

				void fire(transition_t& t)
				{
					std::vector<int> diff(mStates.size());

					mChanged = true;

					for(auto& r : std::get<T_FROM>(t))
					{
						mStates.at(r.first) -= r.second;
						diff[r.first] -= r.second;
					}

					for(auto& r : std::get<T_TO>(t))
					{
						mStates.at(r.first) += r.second;
						diff[r.first] += r.second;
					}

					for(uint i = 0 ; i < diff.size() ; ++i)
					{
						if(diff[i] < 0)
						{
							call(i);
						}
					}

					if(static_cast<bool>(std::get<T_F>(t)))
					{
						std::get<T_F>(t)();
					}

					for(uint i = 0 ; i < diff.size() ; ++i)
					{
						if(diff[i] > 0)
						{
							call(i);
						}
					}
				}

				void call(uint s)
				{
					auto i = mWatchers.find(s);

					if(i != mWatchers.end())
					{
						i->second();
					}
				}

			private:
				const T mEmpty;
				states_t mStates;
				transitions_t mTransitions;
				watch_table_t mWatchers;
				bool mChanged;
		};
	}
}

#endif
