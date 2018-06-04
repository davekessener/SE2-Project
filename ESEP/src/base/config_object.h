
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
				static constexpr float TOLERANCE = 0.05; // Tolerance for all sensor values
				MXT_DEFINE_E(InvalidObjectException);
				MXT_DEFINE_E(CouldNotOpenFileException);
				MXT_DEFINE_E(InvalidDataException);

			public:
				ConfigObject(const std::string& path = "system.conf");
				void save();
				bool isValid();

				uint16_t heightSensorMin();
				uint16_t heightSensorMax();
				uint32_t startToHs();
				uint32_t hsToSwitch();
				uint32_t switchToEnd();
				uint32_t maxHandOverTime();
				float timeTolerance();

				void setHeightSensorMin(uint16_t);
				void setHeightSensorMax(uint16_t);
				void setStartToHs(uint32_t);
				void setHsToSwitch(uint32_t);
				void setSwitchToEnd(uint32_t);
				void setMaxHandOverTime(uint32_t);
				void setTimeTolerance(float);

			private:
				const std::string mPath;
				bool mValid;
				uint16_t mHeightSensorMin;		// Minimal height value [int value]
				uint16_t mHeightSensorMax;		// Maximal height value [int value]
				uint32_t mStartToHs;			// Time between LB_Start and LB_HeightSensor [ms]
				uint32_t mHsToSwitch;			// Time between LB_HeightSensor and LB_Switch [ms]
				uint32_t mSwitchToEnd;			// Time between LB_Switch and LB_End [ms]
				uint32_t mMaxHandOverTime;	// Max Time for handing over an ITEM
				float mTimeTolerance;			// TimeTolerance depending on ITEM going either the short or the long way [%]
		};

}
}

#endif
