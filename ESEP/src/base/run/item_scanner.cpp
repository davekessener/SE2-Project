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
	height_t h = static_cast<height_t>(height);
	if(!mSuspend)
	{
		time_t hvalStamp = static_cast<time_t>(lib::Timer::instance().elapsed());
		if(!static_cast<bool>(mCurrentHM))
		{
			MXT_LOG_INFO("ItemScanner: Started a new hightmap!");
			mFirstStamp = hvalStamp;
			mCurrentHM.reset(new data::HeightMap);
			mCurrentHM->addHeightValue(0, h);
		}
		else if (h == 0)
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
			mCurrentHM->addHeightValue(hvalStamp-mFirstStamp, h);
		}
	}
}

void ItemScanner::finishMeasurement()
{
	if(mCurrentHM->size() > mLB && mCurrentHM->size() < mUB)
	{
		MXT_LOG_INFO("ItemScanner: Finished one hightmap with ", mCurrentHM->size(), " values.");
		mFinishedHM.emplace_back(mCurrentHM.release());
	}
	else
	{
		MXT_LOG_WARN("ItemScanner: Measured heightmap got: ", mCurrentHM->size(), " values! \tLowerbound: ", mLB,", Upperbound: ", mUB);
	}

	mCurrentHM.reset();
	mFirstStamp = 0;
	mLastMappingTime = 0;
}

void ItemScanner::suspend()
{
	mSuspend = true;

	mLastMappingTime = (static_cast<time_t> (lib::Timer::instance().elapsed())) - mFirstStamp;
}

void ItemScanner::resume()
{
	if(static_cast<bool>(mCurrentHM))
	{
		mFirstStamp = (static_cast<time_t> (lib::Timer::instance().elapsed())) - mLastMappingTime;
	}
	mSuspend = false;
}

data::Data_ptr ItemScanner::getHeightmap()
{
	if(mFinishedHM.empty())
	{
		MXT_THROW_EX(NoMeasuredHeighMap);
		return nullptr;
	}

	Data_ptr d = mFinishedHM.front();
	mFinishedHM.pop_front();
	return d;
}


}}}




