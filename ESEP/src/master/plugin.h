#ifndef ESEP_MASTER_PLUGIN_H
#define ESEP_MASTER_PLUGIN_H

#include <vector>

#include "data/data_point.h"

#include "lib/analyse/profiles.h"

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
				UPSIDEDOWN,
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

		namespace plugin
		{
			typedef analyse::Profiles::Item Profile;

			template<Plugin::Type T, Profile P> struct Assoc { static constexpr Profile Value = P; };

			typedef tml::MakeTypeList<
				Assoc<Plugin::Type::FLAT,         Profile::FLAT>,
				Assoc<Plugin::Type::HOLLOW,       Profile::HOLLOW>,
				Assoc<Plugin::Type::HOLLOW_METAL, Profile::HOLLOW>,
				Assoc<Plugin::Type::UPSIDEDOWN,   Profile::UPSIDEDOWN>,
				Assoc<Plugin::Type::CODED_000,    Profile::CODED_000>,
				Assoc<Plugin::Type::CODED_001,    Profile::CODED_001>,
				Assoc<Plugin::Type::CODED_010,    Profile::CODED_010>,
				Assoc<Plugin::Type::CODED_011,    Profile::CODED_011>,
				Assoc<Plugin::Type::CODED_100,    Profile::CODED_100>,
				Assoc<Plugin::Type::CODED_101,    Profile::CODED_101>,
				Assoc<Plugin::Type::CODED_110,    Profile::CODED_110>,
				Assoc<Plugin::Type::CODED_111,    Profile::CODED_111>
			> profile_lookup_t;

			template<Plugin::Type T> struct LookupImpl
			{
				template<typename TT> struct Apply : tml::False { };
				template<Profile P> struct Apply<Assoc<T, P>> : tml::True { };
			};

			template<Plugin::Type T>
			using LookupProfile = tml::list::DoFindFirst<profile_lookup_t, LookupImpl<T>>;
		}
	}
}

#endif
