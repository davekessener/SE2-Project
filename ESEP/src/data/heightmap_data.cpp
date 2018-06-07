#include "data/heightmap_data.h"

#include "data/data_manager.h"

namespace esep { namespace data {

void HeightMap::doSerialize(lib::ByteStream& bs) const
{
	for(const auto& p : *this)
	{
		bs << p.first;
		bs << p.second;
	}
}

void HeightMap::addHeightValue(time_t time, height_t height)
{
	mHeightValues.emplace_back(value_type(time, height));
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
		time_t time;
		height_t height;

		bs >> time;
		bs >> height;

		hm->addHeightValue(time, height);
	}

	return Data_ptr(hm.release());

}

}}


