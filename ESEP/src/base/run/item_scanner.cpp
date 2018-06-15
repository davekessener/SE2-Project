#include "item_scanner.h"
#include "lib/logger.h"
#include "data/data_point.h"

#define MXT_FINISH_DELAY 15

namespace esep { namespace base { namespace run {

ItemScanner::ItemScanner(size_t ub)
	:	mUB(ub)
	, mSuspend(false)
	, mDone(false)
	, mFirstStamp(0)
	, mLastMappingTime(0)
{
}

void ItemScanner::takeMeasurement(height_t h)
{
	if(!mSuspend)
	{
		auto t = lib::Timer::instance().elapsed();

		if(h)
		{
			if(!static_cast<bool>(mCurrentHM))
			{
				MXT_LOG_INFO("Started a new hightmap!");

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
				}, MXT_FINISH_DELAY);
			}
		}
	}
}

void ItemScanner::finishMeasurement()
{
	if(mCurrentHM->size() <= mUB)
	{
		MXT_LOG_INFO("Finished one hightmap with ", mCurrentHM->size(), " values.");

		mFinishedHM.emplace_back(mCurrentHM.release());
	}
	else
	{
		MXT_LOG_ERROR("Measured heightmap got: ", mCurrentHM->size(), " (> ", mUB, ") values!");
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




