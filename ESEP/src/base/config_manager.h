#ifndef SRC_BASE_CONFIG_MANAGER_H
#define SRC_BASE_CONFIG_MANAGER_H

#include "base/IManager.h"
#include "base/config_object.h"

#include "hal/hal.h"

namespace esep
{
	namespace base
	{
		class ConfigManager : public IManager
		{
			typedef communication::Packet Packet;
			typedef communication::Packet_ptr Packet_ptr;
			typedef communication::Message Message;
			typedef Packet::Location Location;

			static constexpr uint T_PERIOD = 2000;

			enum class State
			{
				STATE_0, STATE_1, STATE_2, STATE_3,
				STATE_4, STATE_5, STATE_6, STATE_7,
				STATE_8, STATE_9, STATE_10, STATE_11
			};

			public:
				ConfigManager(communication::IRecipient *, ConfigObject *);
				void enter() override;
				void leave() override;
				void handle(Event) override;
				void accept(Packet_ptr) override;

			private:
				communication::IRecipient * const mHandler;
				ConfigObject * const mConfig;
				State mState;
				uint64_t mTimestamp;
				uint64_t mTimestamp2;
				uint16_t mHeightSensorMin;
				uint16_t mHeightSensorMax;
				uint32_t mItemInLB;
				uint32_t mStartToEnd;
				uint32_t mStartToEndLong;
				uint32_t mStartToEndSlow;
				uint32_t mStartToHs;
				uint32_t mHsToSwitch;
				uint32_t mSwitchToEnd;
				float mTimeTolerance;
		};
}
}

#endif
