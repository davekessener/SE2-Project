#ifndef ESEP_LIB_ARGUMENTS_H
#define ESEP_LIB_ARGUMENTS_H

#include <string>
#include <map>

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		class Arguments
		{
			public:
			MXT_DEFINE_E(InvalidArgumentException);

			public:
				template<typename I>
				Arguments(I&& i1, I&& i2)
				{
					while(i1 != i2)
					{
						process(*i1++);
					}
				}

				bool has(const std::string& id) const { return mArgs.find(id) != mArgs.end(); }
				std::string get(const std::string& id) const { return mArgs.find(id)->second; }

			private:
				void process(const std::string&);

			private:
				std::map<std::string, std::string> mArgs;
		};
	}
}

#endif
