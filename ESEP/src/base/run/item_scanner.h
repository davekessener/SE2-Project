#ifndef SRC_BASE_RUN_ITEM_SCANNER_H
#define SRC_BASE_RUN_ITEM_SCANNER_H

#include <deque>
#include <utility>

#include "communication/packet.h"
#include "data/heightmap_data.h"
#include "data/data_point.h"
#include "system.h"
#include "hal.h"

namespace esep
{
	namespace base
	{
		namespace run
		{
			class ItemScanner
			{
				MXT_DEFINE_E(NoMeasuredHeighMap);
				static constexpr uint32_t UPPERBOUND = 500;
				static constexpr uint32_t LOWERBOUND = 100;

				typedef data::Data_ptr Data_ptr;
				typedef std::unique_ptr<data::HeightMap> heightMap_ptr;

				public:
					ItemScanner(uint32_t lb = LOWERBOUND, uint32_t ub = UPPERBOUND);
					void takeMeasurement(uint16_t height);
					Data_ptr getHeightmap(void);
					void suspend(void);
					void resume(void);

				private:
					void finishMeasurement(void);

				private:
					uint32_t mUB;
					uint32_t mLB;

					std::deque<Data_ptr> mFinishedHM;
					heightMap_ptr mCurrentHM;
					timer::Manager mFinishTimer;
					bool mSuspend;
					uint64_t mFirstStamp;
					uint64_t mLastMappingTime;
			};
		}
	}
}


#endif
