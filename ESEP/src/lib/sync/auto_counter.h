#ifndef ESEP_SYNC_AUTOCOUNTER_H
#define ESEP_SYNC_AUTOCOUNTER_H

namespace esep
{
	namespace sync
	{
		/**
		 * Automatic counter utility.
		 * Increments an object in constructor and
		 * decrements it in a destructor
		 */
		template<typename T>
		class AutoCounter
		{
			public:
				AutoCounter(T& v) : mV(v) { ++mV; }
				~AutoCounter( ) { --mV; }
			private:
				T &mV;
		};
	}
}

#endif
