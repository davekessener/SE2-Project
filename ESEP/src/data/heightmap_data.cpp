#include "data/heightmap_data.h"

#include "data/data_manager.h"

namespace esep { namespace data {

void HeightMap::doSerialize(lib::ByteStream& bs) const
{
	for(const auto& p : mHeightValue)
	{
		bs << p.first;
		bs << p.second;
	}
}

void HeightMap::addHeightValue(time_t time, height_t height)
{
	mHeightValue.emplace_back(std::make_pair(time, height));
}

Data_ptr HeightMap::deserialize(lib::ByteStream& bs)
{
	if(bs.empty())
		{
			MXT_THROW_EX(DataManager::UnexpectedEOSException);
		}

	std::unique_ptr<HeightMap> hm(new HeightMap);

	while(!bs.empty())
	{
		uint32_t time;
		bs >> time;
		uint16_t height;
		bs >> height;
		hm->addHeightValue(time, height);
	}

	return Data_ptr(hm.release());

}

}}


