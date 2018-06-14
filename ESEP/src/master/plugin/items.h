#ifndef ESEP_MASTER_PLUGIN_ITEMS_H
#define ESEP_MASTER_PLUGIN_ITEMS_H

#include "master/plugin/coded.h"
#include "master/plugin/sortable.h"

#include "data/metalsensor_data.h"

namespace esep
{
	namespace master
	{
		namespace plugin
		{
			typedef Coded<Plugin::Type::CODED_000, Plugin::Action::KEEP>   Coded_000;
			typedef Coded<Plugin::Type::CODED_001, Plugin::Action::TOSS_M> Coded_001;
			typedef Coded<Plugin::Type::CODED_010, Plugin::Action::TOSS_S> Coded_010;
			typedef Coded<Plugin::Type::CODED_011, Plugin::Action::KEEP>   Coded_011;
			typedef Coded<Plugin::Type::CODED_100, Plugin::Action::TOSS_M> Coded_100;
			typedef Coded<Plugin::Type::CODED_101, Plugin::Action::KEEP>   Coded_101;
			typedef Coded<Plugin::Type::CODED_110, Plugin::Action::KEEP>   Coded_110;
			typedef Coded<Plugin::Type::CODED_111, Plugin::Action::TOSS_S> Coded_111;

			typedef Sortable<Plugin::Type::HOLLOW, Plugin::Type::HOLLOW_METAL, Plugin::Type::HOLLOW_METAL> SortOrder;

			template<Plugin::Type T, typename SortingPolicy>
			class ConcretePlugin : public Hausdorff, public SortingPolicy
			{
				static constexpr Profile PROFILE = LookupProfile<T>::Value;

				public:
					ConcretePlugin(Hausdorff::processor_t *p) : Plugin(T), Hausdorff(p, T, PROFILE), SortingPolicy(T) { }
			};

			template<Plugin::Type T>
			class HollowItem : public ConcretePlugin<T, SortOrder>
			{
				typedef ConcretePlugin<T, SortOrder> Super;
				typedef typename Super::data_t data_t;

				public:
					HollowItem(Hausdorff::processor_t *p) : Plugin(T), Super(p) { }

					float match(const data_t& data) override
					{
						float r = 0.0;

						for(const auto& p : data)
						{
							if(p->type() == data::DataPoint::Type::METAL_SENSOR)
							{
								if(static_cast<data::MetalSensor&>(*p).isMetal() == (T == Plugin::Type::HOLLOW_METAL))
								{
									r = Super::match(data);
								}
							}
						}

						return r;
					}
			};

			typedef ConcretePlugin<Plugin::Type::FLAT, Constant<Plugin::Action::TOSS_M>> Flat;
			typedef ConcretePlugin<Plugin::Type::UPSIDEDOWN, Constant<Plugin::Action::TOSS>> UpsideDown;
			typedef HollowItem<Plugin::Type::HOLLOW> Hollow;
			typedef HollowItem<Plugin::Type::HOLLOW_METAL> HollowMetal;
		}
	}
}

#endif
