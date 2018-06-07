#ifndef ESEP_MASTER_TYPES_H
#define ESEP_MASTER_TYPES_H

#include "lib/utils.h"

#include "communication/packet.h"

namespace esep
{
	namespace master
	{
		typedef uint32_t id_t;
		typedef uint32_t event_t;

		class Event
		{
			typedef communication::Packet Packet;
			typedef communication::Packet_ptr Packet_ptr;
			typedef Packet::Location Location;
			typedef Packet::msg_t msg_t;

			static constexpr uint OFF = sizeof(Packet::msg_t) * 8;

			public:
			enum
			{
				ERROR  = (1 << (OFF + sizeof(Packet::Location) * 8)),
				BASE   = static_cast<uint>(Packet::Location::BASE)   << OFF,
				MASTER = static_cast<uint>(Packet::Location::BASE_M) << OFF,
				SLAVE  = static_cast<uint>(Packet::Location::BASE_S) << OFF
			};

			static event_t fromParts(Location l, msg_t m) { return (static_cast<uint>(l) << OFF) | static_cast<uint>(m); }
			static event_t fromPacket(const Packet_ptr& p) { return fromParts(p->source(), p->message()); }

			private:
				Event( ) = delete;
				Event(const Event&) = delete;
				~Event( ) = delete;
				Event& operator=(const Event&) = delete;
		};
	}
}

#endif
