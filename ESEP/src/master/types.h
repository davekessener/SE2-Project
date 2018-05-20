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

			static constexpr uint OFF = sizeof(Packet::msg_t) * 8;

			public:
			static constexpr event_t ERROR  = (1 << (OFF + sizeof(Packet::Location) * 8));
			static constexpr event_t BASE   = static_cast<uint>(Packet::Location::BASE)   << OFF;
			static constexpr event_t MASTER = static_cast<uint>(Packet::Location::BASE_M) << OFF;
			static constexpr event_t SLAVE  = static_cast<uint>(Packet::Location::BASE_S) << OFF;

			static event_t fromPacket(const Packe_ptr& p) { return (static_cast<uint>(p->source()) << OFF) | static_cast<uint>(p->message()); }

			private:
				Event( ) = delete;
				Event(const Event&) = delete;
				~Event( ) = delete;
				Event& operator=(const Event&) = delete;
		};
	}
}

#endif
