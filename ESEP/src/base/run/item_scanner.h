#ifndef ESEP_BASE_RUN_ITEMSCANNER_H
#define ESEP_BASE_RUN_ITEMSCANNER_H

#include <deque>

#include "communication/packet.h"

#include "data/heightmap_data.h"
#include "data/data_point.h"

#include "hal.h"

namespace esep
{
	namespace base
	{
		namespace run
		{
			class ItemScanner
			{
				public:
				MXT_DEFINE_E(NoMeasuredHeighMap);

				private:
				static constexpr size_t UPPERBOUND = 500;

				typedef data::Data_ptr Data_ptr;
				typedef std::unique_ptr<data::HeightMap> hm_ptr;
				typedef data::HeightMap::height_t height_t;

				public:
					ItemScanner(size_t ub = UPPERBOUND);
					void takeMeasurement(height_t height);
					Data_ptr getHeightmap(void);
					bool ready( ) const { return !mFinishedHM.empty(); }
					void suspend(void);
					void resume(void);

				private:
					void finishMeasurement(void);

				private:
					size_t mUB;
					std::deque<Data_ptr> mFinishedHM;
					hm_ptr mCurrentHM;
					timer::Manager mFinishTimer;
					bool mSuspend, mDone;
					uint64_t mFirstStamp;
					time_t mLastMappingTime;
			};
		}
	}
}


#endif
