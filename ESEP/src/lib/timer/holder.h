#ifndef ESEP_TIMER_HOLDER_H
#define ESEP_TIMER_HOLDER_H

#include "lib/timer/types.h"
#include "lib/timer/base.h"

namespace esep
{
	namespace timer
	{
		class Holder : public Base
		{
			public:
				Holder(id_t id, callback_fn f, uint r, uint p) : Base(id, r, p), mF(std::move(f)) { }

			protected:
				void execute( ) override { mF(); }

			private:
				callback_fn mF;
		};
	}
}

#endif
