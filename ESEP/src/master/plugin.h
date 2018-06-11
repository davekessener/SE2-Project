#ifndef ESEP_MASTER_PLUGIN_H
#define ESEP_MASTER_PLUGIN_H

#include <vector>

#include "data/data_point.h"

namespace esep
{
	namespace master
	{
		class Plugin
		{
			public:
			enum class Action : uint8_t
			{
				VOID,
				KEEP,
				TOSS_M,
				TOSS_S,
				TOSS
			};

			enum class Type : uint8_t
			{
				UNKNOWN,
				DEFAULT,
				FLAT,
				HOLLOW,
				HOLLOW_METAL,
				CODED_000,
				CODED_001,
				CODED_010,
				CODED_011,
				CODED_100,
				CODED_101,
				CODED_110,
				CODED_111
			};

			typedef std::vector<data::Data_ptr> data_t;
			typedef std::vector<Type> history_t;

			public:
				Plugin(Type t) : mType(t) { }
				virtual ~Plugin( ) { }
				Type type( ) const { return mType; }

				virtual float match(const data_t&) = 0;
				virtual Action decide(const history_t&) = 0;

			private:
				const Type mType;
		};

		typedef std::unique_ptr<Plugin> Plugin_ptr;
	}
}

#endif
