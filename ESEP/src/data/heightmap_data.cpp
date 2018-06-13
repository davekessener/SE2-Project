#include "data/heightmap_data.h"

#include "data/data_manager.h"

#include "lib/logger.h"

namespace esep { namespace data {

void HeightMap::doSerialize(lib::ByteStream& bs) const
{
	uint32_t len = mHeightValues.size();

	bs << len;

	for(const auto& p : *this)
	{
		bs << p.first << p.second;
	}
}

void HeightMap::addHeightValue(time_t time, height_t height)
{
	mHeightValues.emplace_back(time, height);
}

Data_ptr HeightMap::deserialize(lib::ByteStream& bs)
{
	if(bs.empty())
	{
		MXT_THROW_EX(DataManager::UnexpectedEOSException);
	}

	std::unique_ptr<HeightMap> hm(new HeightMap);

	uint32_t len;

	bs >> len;

	if(bs.size() < len)
	{
		// TODO
		throw 0;
	}

	while(len--)
	{
		time_t time;
		height_t height;

		bs >> time;
		bs >> height;

		hm->addHeightValue(time, height);
	}

	return Data_ptr(hm.release());

}

}}


