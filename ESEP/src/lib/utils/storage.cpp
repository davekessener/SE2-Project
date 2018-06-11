#include "lib/utils/storage.h"

#include "lib/utils/items/item_flat_1.h"
#include "lib/utils/items/item_flat_2.h"
#include "lib/utils/items/item_hollow_1.h"
#include "lib/utils/items/item_hollow_2.h"
#include "lib/utils/items/item_hollow_metal_1.h"
#include "lib/utils/items/item_hollow_metal_2.h"
#include "lib/utils/items/item_new_bin_010_1.h"
#include "lib/utils/items/item_new_bin_010_2.h"
#include "lib/utils/items/item_new_bin_101_1.h"
#include "lib/utils/items/item_new_bin_101_2.h"
#include "lib/utils/items/item_new_bin_110_1.h"
#include "lib/utils/items/item_new_bin_110_2.h"
#include "lib/utils/items/item_old_bin_010_1.h"
#include "lib/utils/items/item_old_bin_010_2.h"
#include "lib/utils/items/item_old_bin_101_1.h"
#include "lib/utils/items/item_old_bin_101_2.h"
#include "lib/utils/items/item_old_bin_110_1.h"
#include "lib/utils/items/item_old_bin_110_2.h"
#include "lib/utils/items/item_unknown.h"

#include "data/heightmap_data.h"

#include "master/plugin.h"

namespace esep { namespace lib { namespace utils { namespace storage {

namespace
{
	data::HeightMap *generate(const uint16_t *raw, size_t n)
	{
		std::unique_ptr<data::HeightMap> hm(new data::HeightMap);

		while(n--)
		{
			auto t = *raw++;
			auto v = *raw++;

			hm->addHeightValue(t, v);
		}

		return hm.release();
	}
}

Impl::Impl(void)
{
#define CREATE(id, s) store( # s, std::make_pair(master::Plugin::Type::id, generate(s::DATA(), s::SIZE() / 2)));
	CREATE(FLAT, item_flat_1);
	CREATE(HOLLOW, item_hollow_1);
	CREATE(HOLLOW, item_hollow_metal_1);
	CREATE(CODED_010, item_new_bin_010_1);
	CREATE(CODED_101, item_new_bin_101_1);
	CREATE(CODED_110, item_new_bin_110_1);
	CREATE(CODED_010, item_old_bin_010_1);
	CREATE(CODED_101, item_old_bin_101_1);
	CREATE(CODED_110, item_old_bin_110_1);
	CREATE(FLAT, item_flat_2);
	CREATE(HOLLOW, item_hollow_2);
	CREATE(HOLLOW, item_hollow_metal_2);
	CREATE(CODED_010, item_new_bin_010_2);
	CREATE(CODED_101, item_new_bin_101_2);
	CREATE(CODED_110, item_new_bin_110_2);
	CREATE(CODED_010, item_old_bin_010_2);
	CREATE(CODED_101, item_old_bin_101_2);
	CREATE(CODED_110, item_old_bin_110_2);
#undef CREATE
}

}}}}
