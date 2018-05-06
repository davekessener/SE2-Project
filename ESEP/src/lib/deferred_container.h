#ifndef ESEP_LIB_DEFERREDCONTAINER_H
#define ESEP_LIB_DEFERREDCONTAINER_H

#include <list>
#include <forward_list>
#include <map>

namespace esep
{
	namespace lib
	{
		template<typename K, typename V>
		class DeferredContainer
		{
			typedef K key_type;
			typedef V value_type;
			typedef std::map<key_type, value_type> container_t;
			typedef typename container_t::iterator iterator;
			typedef std::function<void(value_type&)> callback_t;

			public:
				template<typename TT>
				void insert(const key_type& k, TT&& v)
				{
					mToInsert.emplace(std::make_pair(k, std::forward<TT>(v)));
				}

				void remove(const key_type& k)
				{
					mToRemove.emplace_front(k);
				}

				void update(void)
				{
					for(auto& i : mToInsert)
					{
						mContainer.emplace(std::make_pair(i.first, std::move(i.second)));
					}

					for(auto& i : mToRemove)
					{
						auto j = mContainer.find(i);

						if(j != mContainer.end())
						{
							mContainer.erase(j);
						}
					}

					for(auto& i : mCallbacks)
					{
						auto j = mContainer.find(i.first);

						if(j != mContainer.end())
						{
							i.second(j->second);
						}
					}

					mToInsert.clear();
					mToRemove.clear();
					mCallbacks.clear();
				}

				void with(const key_type& k, callback_t f)
				{
					mCallbacks.emplace(std::make_pair(k, std::move(f)));
				}

				iterator begin( ) { return mContainer.begin(); }
				iterator end( ) { return mContainer.end(); }
				iterator erase(iterator i) { return mContainer.erase(i); }

			private:
				container_t mContainer;
				std::map<key_type, value_type> mToInsert;
				std::map<key_type, callback_t> mCallbacks;
				std::forward_list<key_type> mToRemove;
		};
	}
}

#endif
