#ifndef ESEP_MASTER_PLUGIN_HAUSDORFFITEM_H
#define ESEP_MASTER_PLUGIN_HAUSDORFFITEM_H

#include "data/heightmap_data.h"

#include "lib/analyse/hausdorff.h"
#include "lib/analyse/profiles.h"
#include "lib/analyse/preprocessor.h"
#include "lib/analyse/v.h"

#include "lib/process_tree.h"
#include "lib/processor.h"

#include "master/plugin.h"

namespace esep
{
	namespace master
	{
		namespace plugin
		{
			namespace impl
			{
				typedef data::HeightMap *key_type;

				struct Hausdorff
				{
					typedef analyse::Hausdorff<analyse::vec2> hd_t;
					typedef std::vector<analyse::vec2> container_type;

					hd_t operator()(const container_type& c) { return hd_t(c); }
				};
			}

			class Hausdorff : public virtual Plugin
			{
				typedef analyse::Profiles::Item profile_type;
				typedef analyse::Profiles::profile_t profile_t;

				public:
				enum class Station
				{
					NORMALIZED,
					HAUSDORFF
				};

				private:
				template<Station S, typename T>
				using Tag = lib::data::Tagger<Station>::Tag<S, T>;

				template<typename ... S>
				using Source = lib::data::Source<S...>;

				typedef lib::data::ProcessTree<Station, impl::key_type,
					Source<
						Tag<Station::NORMALIZED, analyse::Preprocessor<analyse::vec2>>,
						Tag<Station::HAUSDORFF, impl::Hausdorff>>>
				process_tree_t;

				public:
				typedef lib::Processor<impl::key_type, process_tree_t> processor_t;

				public:
					Hausdorff(processor_t * p, Type t, profile_type hdp)
						: Plugin(t)
						, mProfile(analyse::Profiles::get(hdp))
						, mProcessor(p) { };

					float match(const data_t&) override;

				protected:
					process_tree_t& use(const impl::key_type& k) { return mProcessor->use(k); }

				private:
					const profile_t& mProfile;
					processor_t * const mProcessor;
			};

			template<analyse::Profiles::Item T>
			class Profiled : public Hausdorff
			{
				public:
					Profiled(processor_t *p, Type t) : Hausdorff(p, t, T) { }
			};
		}
	}
}

#endif
