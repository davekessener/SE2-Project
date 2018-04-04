#ifndef ESEP_LIB_OBSERVER
#define ESEP_LIB_OBSERVER

#include <functional>
#include <map>

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		class Observable
		{
			public:
			typedef std::function<void(Observable*)> callback_t;
			typedef uint id_t;

			public:
				Observable( );
				virtual ~Observable( );
				id_t subscribe(callback_t);
				void unsubscribe(id_t);
			protected:
				void notifyAll( );
			private:
				std::map<id_t, callback_t> mObservers;
				uint mNextID;
		};
	}
}

#endif

