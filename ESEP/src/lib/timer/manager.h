#ifndef ESEP_TIMER_MANAGER_H
#define ESEP_TIMER_MANAGER_H

#include "lib/timer/types.h"

namespace esep
{
	namespace timer
	{
		class Manager
		{
			public:
				Manager( ) : mID(INVALID_TIMER_ID) { }
				Manager(Manager&&);
				~Manager( );
				Manager& operator=(Manager&&);
				void swap(Manager&) noexcept;
				void reset( ) { Manager tm; swap(tm); }
			private:
				Manager(id_t id) : mID(id) { }

			private:
				id_t mID;

			private:
				Manager(const Manager&) = delete;
				Manager& operator=(const Manager&) = delete;

				friend class Impl;
		};
	}
}

#endif
