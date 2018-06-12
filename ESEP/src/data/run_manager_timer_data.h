#ifndef ESEP_DATA_RUN_MANAGER_TIMER_H
#define ESEP_DATA_RUN_MANAGER_TIMER_H

#include "data/data_point.h"

namespace esep
{
	namespace data
	{
		class RunManagerTimer : public DataPoint
		{

			public:

			enum class TimerEvent : uint8_t
			{
				EXPECT_NEW,
				START_1,
				ITEM_READY_HS,
				START_2,
				HS_1,
				ITEM_READY_SWITCH,
				HS_2,
				SWITCH_1,
				RAMP,
				SWITCH_2,
				ITEM_READY_END,
				SWITCH_3,
				CLOSE_SWITCH
			};

				RunManagerTimer(TimerEvent v) : DataPoint(DataPoint::Type::RUN_MANAGER_TIMER), mEvent(v) { }
				TimerEvent event( ) const { return mEvent; }

				std::string to_s( ) const override { return lib::stringify("T", lib::hex<sizeof(mEvent) * 8>(mEvent)); }

				static Data_ptr deserialize(lib::ByteStream&);

			protected:
				void doSerialize(lib::ByteStream&) const override;

			private:
				const TimerEvent mEvent;
		};
	}
}
#endif
