#include "item_scanner.h"
#include "lib/logger.h"
#include "data/data_point.h"

namespace esep { namespace base { namespace run {

ItemScanner::ItemScanner()
	: mSuspend(false)
	, mDone(false)
	, mFirstStamp(0)
	, mLastMappingTime(0)
{
}

void ItemScanner::takeMeasurement(uint16_t h)
{
	if(!mSuspend)
	{
		uint64_t t = lib::Timer::instance().elapsed();

		if(h)
		{
			if(!static_cast<bool>(mCurrentHM))
			{
				MXT_LOG_INFO("ItemScanner: Started a new hightmap!");

				mFirstStamp = t;
				mCurrentHM.reset(new data::HeightMap);
			}

			mDone = false;
			mFinishTimer.reset();
			mCurrentHM->addHeightValue(t - mFirstStamp, h);
		}
		else if(static_cast<bool>(mCurrentHM))
		{
			if(!mDone)
			{
				mDone = true;
				mFinishTimer = lib::Timer::instance().registerCallback([this](void) {
					finishMeasurement();
				}, 10);
			}
		}
	}
}

void ItemScanner::finishMeasurement()
{
	if(mCurrentHM->size() > LOWERBOUND && mCurrentHM->size() < UPPERBOUND)
	{
		MXT_LOG_INFO("ItemScanner: Finished one hightmap with ", mCurrentHM->size(), " values.");

		mFinishedHM.emplace_back(mCurrentHM.release());
	}
	else
	{
		uint32_t L = LOWERBOUND;
		uint32_t U = UPPERBOUND;
		MXT_LOG_WARN("ItemScanner: Measured heightmap got: ", mCurrentHM->size(), " values! \tLowerbound: ", L,", Upperbound: ", U);
	}
}

void ItemScanner::suspend()
{
	mSuspend = true;
	mLastMappingTime = lib::Timer::instance().elapsed() - mFirstStamp;
}

void ItemScanner::resume()
{
	mFirstStamp = lib::Timer::instance().elapsed() - mLastMappingTime;
	mSuspend = false;
}

data::Data_ptr ItemScanner::getHeightmap()
{
	if(mFinishedHM.empty())
	{
		MXT_THROW_EX(NoMeasuredHeighMap);
	}

	Data_ptr d = mFinishedHM.front();

	mFinishedHM.pop_front();

	return d;
}


}}}




