
#ifndef TEST_FT_HEIGHTSENSOR_ITEM_MEASUREMENT_H
#define TEST_FT_HEIGHTSENSOR_ITEM_MEASUREMENT_H

#include <stdint.h>
#include <vector>

#include "lib/utils.h"

namespace esep
{
	namespace test
	{
		namespace functional
		{
			namespace h
			{
				class ItemMeasurement
				{
					public:
						MXT_DEFINE_E(CouldNotOpenFileException);

						enum class State {
							CONFIG_IDLE,
							CONFIG_RUNNING,
							IDLE,
							MEASURE_IDLE,
							MEASURE_RUNNING,
							CONFIG_MIN
						};

					public:
						void run( );

					private:
						void measure(uint16_t);
						void save( );
						void clearProfile( );

					private:
						std::vector<std::pair<uint32_t, uint16_t>> mProfile;
						uint32_t mStartTime;

				};
			}
		}
	}
}

#endif
