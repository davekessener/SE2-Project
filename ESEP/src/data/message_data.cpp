#include "data/message_data.h"

namespace esep { namespace data {

void Message::doSerialize(lib::ByteStream& bs) const
{
	uint16_t len = mMsg.size();

	bs << len << lib::byte_stream::insert_all(mMsg);
}

Data_ptr Message::deserialize(lib::ByteStream& bs)
{
	uint16_t len = 0;
	std::vector<char> b;

	bs >> len;

	while(len--) b.push_back(bs.remove());

	return Data_ptr(new Message(std::string(b.begin(), b.end())));
}

}}
