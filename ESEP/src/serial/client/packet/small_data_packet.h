#ifndef ESEP_SERIAL_CLIENT_PACKET_SDP_H
#define ESEP_SERIAL_CLIENT_PACKET_SDP_H

#include <algorithm>

#include "serial/client/packet/base.h"
#include "serial/client/packet/traits.h"

#include "serial/client/m_connection.h"

namespace esep
{
	namespace serial
	{
		namespace packet
		{
			template
			<
				Type T,
				typename CRC = lib::crc::Generator<uint32_t>
			>
			class Data : public Base
			{
				typedef Traits<T> traits_t;

				public:
				static constexpr Type TYPE = T;
				static constexpr uint SIZE = traits_t::Size;
				typedef typename traits_t::crc_type crc_t;
				typedef typename traits_t::size_type length_t;

				public:
					Data(id_t, const types::buffer_t&);
					types::buffer_t getData( ) { return types::buffer_t(mData, mData + mLength); }
					virtual types::buffer_t serialize( );
					static packet_ptr deserialize(modules::In_Connection&);
				private:
					id_t mID;
					byte_t mData[SIZE];
					length_t mLength;
					crc_t mCRC;
			};

			template<Type T, typename CRC>
			Data<T, CRC>::Data(id_t id, const types::buffer_t& buf)
			{
				if(buf.size() > SIZE)
					MXT_THROW_E(PacketDataOverflowException,
							lib::stringify("Tried to fill packet ", T, " with ", buf.size(), " bytes of data!"));

				CRC crcGen;
				mID = id;
				mLength = buf.size();

				std::copy(std::begin(buf), std::end(buf), mData);
				if(mLength < SIZE)
					std::fill(mData + mLength, mData + SIZE, 0);

				mCRC = traits_t::calculateCRC(crcGen.encode(mData));
			}

			template<Type T, typename CRC>
			types::buffer_t Data<T, CRC>::serialize(void)
			{
				types::buffer_t buf;

				buf << static_cast<uint16_t>(T)
					<< mID
					<< mLength
					<< mCRC;

				byte_t checksum = buf.checksum();

				buf << checksum
					<< mData;

				return buf;
			}

			template<Type T, typename CRC>
			packet_ptr Data<T, CRC>::deserialize(modules::In_Connection& c)
			{
				CRC crcGen;
				types::buffer_t header, data;

				header << static_cast<uint16_t>(T)
					   << c.read(sizeof(mID) + sizeof(mLength) + sizeof(mCRC) + 1);

				types::buffer_t raw_data = c.read(SIZE);

				if(header.checksum() != 0)
					throw types::BadPacketException;

				auto crc = crcGen.encode(raw_data);

				for(auto i = raw_data.cbegin() ; )
			}
		}
	}
}

#endif
