#ifndef ESEP_HAL_HAL_H
#define ESEP_HAL_HAL_H

#include <functional>
#include <memory>

#include "lib/utils.h"

namespace esep
{
	namespace hal
	{
		class HAL
		{
			public:
			typedef uint32_t bitmask_t;

			enum class Field : uint
			{
				GPIO_0 = 0,
				GPIO_1 = 1,
				GPIO_2 = 2,
				ANALOG = 3
			};

			static const uint N_FIELDS = 4;

			enum class Event : uint8_t
			{
				LB_START =         2,
				LB_END =          20,
				LB_HEIGHTSENSOR =  3,
				LB_RAMP =         15,
				LB_SWITCH =        5,
				BTN_START =       22,
				BTN_STOP =        23,
				BTN_RESET =       26,
				BTN_ESTOP =       27,
				HEIGHT_SENSOR =   (3ull << 5)
			};

			static constexpr Event EVENTS[] = {
				Event::LB_START,
				Event::LB_END,
				Event::LB_HEIGHTSENSOR,
				Event::LB_RAMP,
				Event::LB_SWITCH,
				Event::BTN_START,
				Event::BTN_STOP,
				Event::BTN_RESET,
				Event::BTN_ESTOP,
				Event::HEIGHT_SENSOR
			};
			static constexpr size_t N_EVENTS = sizeof(EVENTS) / sizeof(Event);

			/**
			 * Type of the event handler used by the HAL class to
			 * react to Events send by the sensors
			 */
			typedef std::function<void(Event)> callback_t;

			static constexpr Field getField(Event e) { return static_cast<Field>(static_cast<uint8_t>(e) >> 5); }
			static constexpr uint32_t getPin(Event e) { return getField(e) == Field::ANALOG ? 0 : (1 << (static_cast<uint8_t>(e) & 0x1F)); }

			public:
				virtual ~HAL( ) { }
				virtual uint32_t in(Field) = 0;
				virtual void out(Field, uint32_t) = 0;
				virtual void set(Field, bitmask_t) = 0;
				virtual void reset(Field, bitmask_t) = 0;
				virtual void setCallback(callback_t) = 0;
			private:
		};

		typedef std::unique_ptr<HAL> HAL_ptr;
	}
}

#endif

