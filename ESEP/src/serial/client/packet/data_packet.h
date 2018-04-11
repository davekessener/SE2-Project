#ifndef ESEP_SERIAL_CLIENT_PACKET_SDP_H
#define ESEP_SERIAL_CLIENT_PACKET_SDP_H

#include <algorithm>

#include "serial/client/packet/base.h"
#include "serial/client/packet/traits.h"

#include "serial/client/m_connection.h"

#include "lib/utils.h"
#include "lib/logger.h"
#include "lib/algorithms.h"

namespace esep
{
	namespace serial
	{
		namespace packet
		{
			class Base_Data : public Base
			{
				public:
					virtual bool checkIntegrity( ) const = 0;
					virtual types::buffer_t getData( ) const = 0;
					virtual bool isChained( ) const = 0;
				private:
			};

			template<Type T>
			class Data : public Base_Data
			{
				typedef Traits<T> traits_t;

				using types::id_t;

				public:
				typedef lib::crc::Generator<uint32_t> CRC;
				static constexpr Type TYPE = T;
				typedef typename traits_t::crc_type crc_t;
				typedef typename traits_t::size_type length_t;
				static constexpr uint SIZE = traits_t::Size;
				static constexpr uint HEADER_SIZE
					= sizeof(Type) + sizeof(id_t) + sizeof(length_t) + sizeof(crc_t) + sizeof(byte_t);

				public:
					template<typename C>
						Data(id_t, C&&, bool = false);
					types::buffer_t getData( ) { return types::buffer_t(mData, mData + mLength); }
					crc_t calculateCRC( ) const { CRC crcGen; return traits::calculateCRC(crcGen.encode(mData)); }
					crc_t getCRC( ) const { return mCRC; }
					virtual bool checkIntegrity( ) const { return mCRC == calculateCRC(); }
					virtual bool isChained( ) const { return mChained; }
					virtual types::buffer_t getData( ) const
						{ types::buffer_t b; b << lib::byte_stream::insert_all(mData); return b; }
					virtual Type getType( ) const { return T; }
					virtual types::buffer_t serialize( );
					static packet_ptr deserialize(modules::In_Connection&);
				private:
					Data(id_t, crc_t, const types::buffer_t&, length_t, bool);
				private:
					byte_t mData[SIZE];
					length_t mLength;
					crc_t mCRC;
					bool mChained;
			};

			template<Type T, typename CRC>
			template<typename C>
			Data<T, CRC>::Data(id_t id, C&& buf, bool chained)
				: Base(id)
			{
				if(buf.size() > SIZE || buf.empty() || (chained && buf.size() != SIZE))
					MXT_THROW_E(PacketDataOverflowException,
							lib::stringify("Tried to fill packet ", T, " with ", buf.size(),
									" bytes of data!", (chained ? " (Chained)" : "")));

				mLength = buf.size();
				mChained = chained;

				std::copy(std::begin(buf), std::end(buf), mData);
				if(mLength < SIZE)
					std::fill(mData + mLength, mData + SIZE, 0);

				mCRC = calculateCRC();
			}

			template<Type T, typename CRC>
			Data<R, CRC>::Data(id_t id, crc_t crc, const types::buffer_t& buf, length_t l, bool chained)
				: Base(id)
			{
				mLength = chained ? SIZE : l;
				mCRC = crc;
				mChained = chained;

				std::copy(std::begin(buf), std::end(buf), mData);
			}

			template<Type T, typename CRC>
			types::buffer_t Data<T, CRC>::serialize(void)
			{
				types::buffer_t buf;

				buf << static_cast<uint16_t>(T)
					<< getID()
					<< (mChained ? static_cast<length_t>(0) : mLength)
					<< mCRC;

				byte_t checksum = lib::checksum(buf);

				buf << checksum
					<< lib::byte_stream::insert_all(mData);

				return buf;
			}

			template<Type T, typename CRC>
			packet_ptr Data<T, CRC>::deserialize(modules::In_Connection& c)
			{
				CRC crcGen;

				types::buffer_t raw_header, raw_data;

				raw_header << static_cast<uint16_t>(T);
				c.read(raw_header, HEADER_SIZE - sizeof(Type));

				c.read(raw_data, SIZE);

				if(lib::checksum(raw_header) != 0)
				{
					throw types::FailedPacketRead();
				}

				uint16_t type;
				id_t id;
				length_t length;
				crc_t crc;
				byte_t checksum;

				raw_header >> type >> id >> length >> crc >> checksum;

				return packet_ptr(new Data<T, CRC>(id, crc, raw_data, length));
			}
		}
	}
}

#endif
