
#ifndef SRC_BASE_CONFIG_OBJECT_H_
#define SRC_BASE_CONFIG_OBJECT_H_

#include <string>
#include <stdexcept>

#include "lib/utils.h"

namespace esep
{
	namespace base
	{

		class ConfigObject
		{
			public:
				MXT_DEFINE_E(InvalidObjectException);
				MXT_DEFINE_E(CouldNotOpenFileException);
				MXT_DEFINE_E(InvalidDataException);

			public:
				ConfigObject(const std::string& path = "system.conf", uint32_t dt = DISCARD_TIME, float ft = TOLERANCE, uint32_t rt = RAMP_TIME, uint32_t ho = HANDOVER_TIME);
				void save();
				bool isValid();

				uint16_t heightSensorMin();
				uint16_t heightSensorMax();
				uint32_t startToHs();
				uint32_t hsToSwitch();
				uint32_t switchToEnd();
				uint32_t itemInLB();
				float timeTolerance();
				uint32_t maxHandOverTime() const { return mMaxHandOverTime; };
				uint32_t rampTime() const { return mRampTime; };
				float tolerance() const { return mFlatTolerance; };
				float discardTime() const { return mDiscardTime; };

				void setHeightSensorMin(uint16_t);
				void setHeightSensorMax(uint16_t);
				void setStartToHs(uint32_t);
				void setHsToSwitch(uint32_t);
				void setSwitchToEnd(uint32_t);
				void setTimeTolerance(float);
				void setItemInLB(uint32_t);

			private:
				const std::string mPath;
				bool mValid;
				uint16_t mHeightSensorMin;		// Minimal height value (its the Value from the floor) [int value]
				uint16_t mHeightSensorMax;		// Maximal height value (its the Value from the top of the highest ITEM)[int value]
				uint32_t mStartToHs;			// Time between LB_Start and LB_HeightSensor [ms]
				uint32_t mHsToSwitch;			// Time between LB_HeightSensor and LB_Switch [ms]
				uint32_t mSwitchToEnd;			// Time between LB_Switch and LB_End [ms]
				uint32_t mMaxHandOverTime;		// Maximal time for handing over an item to another module [ms]
				uint32_t mRampTime;				// Time LB_RAMP has to wait until an event should be sent [ms]
				uint32_t mItemInLB;				// Time an item needs to pass a LB
				float mTimeTolerance;			// TimeTolerance depending on ITEM going either the short or the long way [%]
				const float mFlatTolerance;		// Tolerance for all sensor values
				uint32_t mDiscardTime;			// Time the module need to discard an ITEM

				static constexpr float TOLERANCE = 0.1;
				static constexpr uint32_t HANDOVER_TIME = 1000;
				static constexpr uint32_t RAMP_TIME = 500;
				static constexpr uint32_t DISCARD_TIME = 3000;
		};

}
}

#endif
