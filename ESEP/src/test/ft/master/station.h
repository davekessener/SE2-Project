#ifndef ESEP_TEST_FN_MASTER_STATION_H
#define ESEP_TEST_FN_MASTER_STATION_H

#include "test/ft/master/item.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			namespace m
			{
				class Station
				{
					typedef Item::id_t id_t;

					public:
						Station(uint);
						virtual ~Station( ) { }

						virtual void insert(Item_ptr);

						void tick( );
						bool remove(id_t);
						uint remaining( ) const;

					protected:
						virtual void process(Item_ptr) = 0;

					private:
						const uint mLength;
						std::deque<std::pair<uint, Item_ptr>> mItems;
				};
			}
		}
	}
}

#endif
