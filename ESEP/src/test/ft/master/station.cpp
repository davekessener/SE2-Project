#include "test/ft/master/station.h"

namespace esep { namespace test { namespace functional { namespace m {

Station::Station(uint l)
	: mLength(l)
{
}

void Station::tick(void)
{
	std::vector<Item_ptr> buf;

	for(auto i1 = mItems.begin() ; i1 != mItems.end() ;)
	{
		if(--i1->first)
		{
			++i1;
		}
		else
		{
			buf.push_back(i1->second);

			i1 = mItems.erase(i1);
		}
	}

	for(auto& item : buf)
	{
		process(item);
	}
}

void Station::insert(Item_ptr item)
{
	mItems.emplace_front(std::make_pair(mLength, std::move(item)));
}

bool Station::remove(id_t id)
{
	auto i = std::find_if(mItems.begin(), mItems.end(), [id](const std::pair<uint, Item_ptr>& i) { return i.second->ID() == id; });
	bool r = false;

	if((r = (i != mItems.end())))
	{
		mItems.erase(i);
	}

	return r;
}

uint Station::remaining(void) const
{
	uint r = -1;

	for(const auto& p : mItems)
	{
#define MXT_MIN(a,b) (((a)<(b))?(a):(b))
		r = MXT_MIN(r, p.first);
#undef MXT_MIN
	}

	return r;
}

}}}}
