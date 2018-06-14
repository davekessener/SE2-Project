#include "item_scanner.h"
#include "lib/logger.h"
#include "data/data_point.h"

namespace esep { namespace base { namespace run {

ItemScanner::ItemScanner(uint32_t lb, uint32_t ub)
	:	mUB(ub),
		mLB(lb),
		mCurrentHM(nullptr),
		mSuspend(false),
		mFirstStamp(0),
		mLastMappingTime(0)
{

}

void ItemScanner::takeMeasurement(uint16_t height)
{
	if(!mSuspend)
	{
		uint64_t hvalStamp = lib::Timer::instance().elapsed();
		if(static_cast<bool>(mCurrentHM))
		{
			MXT_LOG_INFO("ItemScanner: Started a new hightmap!");
			mFirstStamp = hvalStamp;
			mCurrentHM.reset(new data::HeightMap);
			mCurrentHM->addHeightValue(0, height);
		}
		else if (height == 0)
		{
			MXT_LOG_INFO("ItemScanner: Got 0 while measuring!");
			mFinishTimer = lib::Timer::instance().registerCallback([this](void)
			{
				finishMeasurement();
			}, 10);
		}
		else
		{
			mFinishTimer.reset();
			mCurrentHM->addHeightValue(hvalStamp-mFirstStamp, height);
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

	mCurrentHM.reset();
	mFirstStamp = 0;
	mLastMappingTime = 0;
}

void ItemScanner::suspend()
{
	mSuspend = true;

	mLastMappingTime = lib::Timer::instance().elapsed() - mFirstStamp;
}

void ItemScanner::resume()
{
	if(static_cast<bool>(mCurrentHM))
	{
		mFirstStamp = lib::Timer::instance().elapsed() - mLastMappingTime;
	}
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




