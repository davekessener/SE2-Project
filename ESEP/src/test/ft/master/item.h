#ifndef ESEP_TEST_FN_MASTER_ITEM_H
#define ESEP_TEST_FN_MASTER_ITEM_H

#include "data/data_point.h"

#include "lib/utils.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			namespace m
			{
				class Item
				{
					typedef data::Data_ptr Data_ptr;
					typedef std::vector<Data_ptr> container_t;
					typedef container_t::iterator iterator;

					public:
					typedef uint id_t;

					public:
						Item(id_t id) : mID(id) { }
						id_t ID( ) const { return mID; }
						void addData(Data_ptr p) { mData.push_back(p); }
						iterator begin( ) { return mData.begin(); }
						iterator end( ) { return mData.end(); }

					private:
						const id_t mID;
						container_t mData;
				};

				typedef std::shared_ptr<Item> Item_ptr;
			}
		}
	}
}

#endif
