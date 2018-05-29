#ifndef ESEP_TEST_FN_MASTER_CALLBACK_H
#define ESEP_TEST_FN_MASTER_CALLBACK_H

#include <functional>

#include "test/ft/master/station.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			namespace m
			{
				class Callback : public Station
				{
					typedef std::function<void(Item_ptr)> cb_fn;

					public:
						Callback(uint l, cb_fn f) : Station(l), mF(f) { }
						void insert(Item_ptr item) override { Station::insert(item); mF(item); }

					private:
						cb_fn mF;
				};
			}
		}
	}
}

#endif
