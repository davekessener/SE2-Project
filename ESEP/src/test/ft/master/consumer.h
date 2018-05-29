#ifndef ESEP_TEST_FN_MASTER_CONSUMER_H
#define ESEP_TEST_FN_MASTER_CONSUMER_H

#include <functional>

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
				class Consumer : public Callback
				{
					typedef std::function<void(Item_ptr)> consume_fn;

					public:
						Consumer(uint l, consume_fn on_in, consume_fn f) : Callback(l, on_in), mConsumer(f) { }

					protected:
						void process(Item_ptr item) override { mConsumer(item); }

					private:
						consume_fn mConsumer;
				};
			}
		}
	}
}

#endif
