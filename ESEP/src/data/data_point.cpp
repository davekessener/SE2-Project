#include "data/data_point.h"

namespace esep { namespace data {

void DataPoint::serialize(lib::ByteStream& bs) const
{
	bs << mType;

	doSerialize(bs);
}

}}
