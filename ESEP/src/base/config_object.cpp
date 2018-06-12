
#include <iostream>
#include <fstream>
#include <vector>

#include "base/config_object.h"

#define MXT_NO_OF_VALUES 7

namespace esep { namespace base {

ConfigObject::ConfigObject(const std::string& path, float ft, uint32_t rt, uint32_t ho)
	: mPath(path)
	, mValid(false)
	, mHeightSensorMin(0)
	, mHeightSensorMax(0)
	, mStartToHs(0)
	, mHsToSwitch(0)
	, mSwitchToEnd(0)
	, mMaxHandOverTime(ho)
	, mRampTime(rt)
	, mItemInLB(0)
	, mTimeTolerance(-1)
	, mFlatTolerance(ft)
{
	std::ifstream confFile;
	confFile.open(mPath);

	std::vector<std::string> fileData;

	if(confFile.is_open())
	{
		std::string line;
		while(std::getline(confFile, line))
		{
			fileData.push_back(line);
		}

		if(fileData.size() != MXT_NO_OF_VALUES)
		{
			MXT_THROW_EX(ConfigObject::InvalidDataException);
		}
		else
		{
			mSwitchToEnd = std::stoi(fileData.back());
			fileData.pop_back();
			mHsToSwitch = std::stoi(fileData.back());
			fileData.pop_back();
			mStartToHs = std::stoi(fileData.back());
			fileData.pop_back();
			mHeightSensorMax = (uint16_t) std::stoi(fileData.back());
			fileData.pop_back();
			mHeightSensorMin = (uint16_t) std::stoi(fileData.back());
			fileData.pop_back();
			mTimeTolerance = (float) std::stof(fileData.back());
			fileData.pop_back();
			mItemInLB = (float) std::stof(fileData.back());
		}
		confFile.close();
	}
}

void ConfigObject::save()
{
	if(!isValid())
	{
		MXT_THROW_EX(InvalidObjectException);
	}

	std::ofstream confFile(mPath);

	if(confFile.is_open())
	{
		confFile << mItemInLB << "\n"
				 << mTimeTolerance << "\n"
				 << mHeightSensorMin << "\n"
				 << mHeightSensorMax << "\n"
				 << mStartToHs    << "\n"
				 << mHsToSwitch   << "\n"
				 << mSwitchToEnd;
		confFile.close();
	}
	else
	{
		MXT_THROW_EX(CouldNotOpenFileException);
	}
}

bool ConfigObject::isValid()
{
	return mValid || (mValid =
			(mHeightSensorMin > 0
			&& mHeightSensorMax > 0
			&& mMaxHandOverTime > 0
			&& mStartToHs > 0
			&& mHsToSwitch > 0
			&& mSwitchToEnd > 0
			&& mTimeTolerance >= 0
			&& mItemInLB > 0));
}

void ConfigObject::setHeightSensorMin(uint16_t val)
{
	if(val == 0)
	{
		MXT_THROW_EX(ConfigObject::InvalidDataException);
	}
	mHeightSensorMin = val;
}

void ConfigObject::setHeightSensorMax(uint16_t val)
{
	if(val == 0)
	{
		MXT_THROW_EX(ConfigObject::InvalidDataException);
	}
	mHeightSensorMax = val;
}

void ConfigObject::setItemInLB(uint32_t val)
{
	if(val == 0)
	{
		MXT_THROW_EX(ConfigObject::InvalidDataException);
	}
	mItemInLB = val;
}

void ConfigObject::setStartToHs(uint32_t val)
{
	if(val == 0)
	{
		MXT_THROW_EX(ConfigObject::InvalidDataException);
	}
	mStartToHs = val;
}

void ConfigObject::setHsToSwitch(uint32_t val)
{
	if(val == 0)
	{
		MXT_THROW_EX(ConfigObject::InvalidDataException);
	}
	mHsToSwitch = val;
}

void ConfigObject::setSwitchToEnd(uint32_t val)
{
	if(val == 0)
	{
		MXT_THROW_EX(ConfigObject::InvalidDataException);
	}
	mSwitchToEnd = val;
}

void ConfigObject::setTimeTolerance(float val)
{
	if(val < 0 || val > 1)
	{
		MXT_THROW_EX(ConfigObject::InvalidDataException);
	}
	mTimeTolerance = val;
}

uint16_t ConfigObject::heightSensorMin(void)
{
	if(!isValid())
	{
		MXT_THROW_EX(ConfigObject::InvalidObjectException);
	}
	return mHeightSensorMin;
}

uint16_t ConfigObject::heightSensorMax(void)
{
	if(!isValid())
	{
		MXT_THROW_EX(ConfigObject::InvalidObjectException);
	}
	return mHeightSensorMax;
}

uint32_t ConfigObject::itemInLB(void)
{
	if(!isValid())
	{
		MXT_THROW_EX(ConfigObject::InvalidObjectException);
	}
	return mItemInLB;
}

uint32_t ConfigObject::startToHs(void)
{
	if(!isValid())
	{
		MXT_THROW_EX(ConfigObject::InvalidObjectException);
	}
	return mStartToHs;
}

uint32_t ConfigObject::hsToSwitch(void)
{
	if(!isValid())
	{
		MXT_THROW_EX(ConfigObject::InvalidObjectException);
	}

	return mHsToSwitch;
}

uint32_t ConfigObject::switchToEnd(void)
{
	if(!isValid())
	{
		MXT_THROW_EX(ConfigObject::InvalidObjectException);
	}
	return mSwitchToEnd;
}

float ConfigObject::timeTolerance(void)
{
	if(!isValid())
	{
		MXT_THROW_EX(ConfigObject::InvalidObjectException);
	}
	return mTimeTolerance;
}

}}
