
#ifndef SRC_BASE_CONFIG_OBJECT_H_
#define SRC_BASE_CONFIG_OBJECT_H_

namespace esep
{
	namespace base
	{

		class ConfigObject
		{
			public:
				static constexpr float TOLERANCE = 0.05; // Tolerance for all sensor values

			public:
				ConfigObject(const std::string& path = "/src/system.conf");
				~ConfigObject();
				void save();
				bool isValid();

				uint16_t heightSensor();	// Average height value [int value]
				uint32_t startToHs();		// Time between LB_Start and LB_HeightSensor [ms]
				uint32_t hsToSwitch();		// Time between LB_HeightSensor and LB_Switch [ms]
				uint32_t switchToEnd();		// Time between LB_Switch and LB_End [ms]
				float slowFactor();			// Slow speed factor
				float backwardFactor();		// Backward speed factor

				void setHeightSensor(uint16_t);
				void setStartToHs(uint32_t);
				void setHsToSwitch(uint32_t);
				void setSwitchToEnd(uint32_t);
				void setSlowFactor(float);
				void setBackwardFactor(float);

			private:
				uint16_t mHeightSensor;
				uint32_t mStartToHs;
				uint32_t mHsToSwitch;
				uint32_t mSwitchToEnd;
				float mSlowFactor;
				float mBackwardFactor;
		};

}
}

#endif
