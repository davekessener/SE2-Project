#ifndef ESEP_TEST_FN_MASTER_FORWARDER_H
#define ESEP_TEST_FN_MASTER_FORWARDER_H

#include "test/ft/master/station.h"
#include "test/ft/master/callback.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			namespace m
			{
				class Forwarder : public Callback
				{
					typedef std::function<void(Item_ptr)> cb_fn;

					public:
						Forwarder(uint l, cb_fn f, Station *s) : Callback(l, f), mNext(s) { }

					protected:
						void process(Item_ptr item) override { mNext->insert(item); }

					private:
						Station * const mNext;
				};
			}
		}
	}
}

#endif
