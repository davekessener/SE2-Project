#ifndef ESEP_LIB_DEFERRED_H
#define ESEP_LIB_DEFERRED_H

#include <functional>

namespace esep
{
	namespace lib
	{
		/**
		 * A Deferred object takes a functor as constructor parameter which it calls during destruction.
		 *
		 * This class can be used for automated cleanup. When a cleanup action has to be
		 * performed in a block that may throw an exception then wrapping that action in
		 * a functor and constructing a Deferred object from it ensures that the cleanup
		 * function is performed whenever the block is exited, whether through normal
		 * control flow, an exception or otherwise.
		 */
		class Deferred
		{
			typedef std::function<void(void)> callback_fn;

			public:
				explicit Deferred(callback_fn f) : mF(f) { }
				~Deferred( ) { mF(); }
			private:
				callback_fn mF;
		};
	}
}

#endif
