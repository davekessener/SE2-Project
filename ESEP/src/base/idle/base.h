#ifndef ESEP_BASE_IDLE_BASE_H
#define ESEP_BASE_IDLE_BASE_H

#include "base/IManager.h"

namespace esep
{
	namespace base
	{
		class IdleBase : public IManager
		{
			static constexpr uint T_MIN = 5;
			static constexpr uint T_MAX = 1000;

			protected:
			typedef communication::Packet Packet;
			typedef communication::Packet_ptr Packet_ptr;
			typedef communication::Message Message;
			typedef Packet::Location Location;


			public:
				IdleBase(uint min = T_MIN, uint max = T_MAX) : mMin(min), mMax(max) { }
				void handle(Event);

				void accept(Packet_ptr) override { }

			protected:
				virtual void shortPress( ) = 0;
				virtual void longPress( ) = 0;

			private:
				const uint mMin, mMax;
				uint64_t mPressed = 0;
		};
	}
}

#endif
