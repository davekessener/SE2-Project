#ifndef ESEP_LIB_UTILS_STORAGE_H
#define ESEP_LIB_UTILS_STORAGE_H

#include <map>

#include "lib/utils.h"
#include "lib/any.h"
#include "lib/singleton.h"

namespace esep
{
	namespace lib
	{
		namespace utils
		{
			namespace storage
			{
				class Impl
				{
					public:
						Impl( );
						Any& retrieve(const std::string& id) { return mData.at(id); }

					private:
						void store(const std::string& id, Any data) { mData[id] = data; }

					private:
						std::map<std::string, Any> mData;
				};
			}

			typedef Singleton<storage::Impl, 0> Storage;
		}
	}
}

#endif
