
#include <iostream>
#include <fstream>
#include <vector>

#include "base/config_object.h"

#define MXT_NO_OF_VALUES 5

namespace esep { namespace base {

ConfigObject::ConfigObject(const std::string& path)
	: mPath(path)
	, mValid(false)
	, mHeightSensor(0)
	, mStartToHs(0)
	, mHsToSwitch(0)
	, mSwitchToEnd(0)
	, mSlowFactor(0)
	, mTimeTolerance(0)
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
			mSlowFactor = std::stof(fileData.back());
			fileData.pop_back();
			mSwitchToEnd = std::stoi(fileData.back());
			fileData.pop_back();
			mHsToSwitch = std::stoi(fileData.back());
			fileData.pop_back();
			mStartToHs = std::stoi(fileData.back());
			fileData.pop_back();
			mHeightSensor = (uint16_t) std::stoi(fileData.back());
			mValid = true;
		}
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
		confFile << mHeightSensor << "\n"
				 << mStartToHs    << "\n"
				 << mHsToSwitch   << "\n"
				 << mSwitchToEnd  << "\n"
				 << mSlowFactor;
	}
	else
	{
		MXT_THROW_EX(CouldNotOpenFileException);
	}
}

bool ConfigObject::isValid()
{
	return mValid || (mValid =
			(mHeightSensor > 0
			&& mStartToHs > 0
			&& mHsToSwitch > 0
			&& mSwitchToEnd > 0
			&& mSlowFactor > 0));
}

void ConfigObject::setHeightSensor(uint16_t val)
{
	if(val == 0)
	{
		MXT_THROW_EX(ConfigObject::InvalidDataException);
	}
	mHeightSensor = val;
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

void ConfigObject::setSlowFactor(float val)
{
	if(val > 1 || val == 0)
	{
		MXT_THROW_EX(ConfigObject::InvalidDataException);
	}
	mSlowFactor = val;
}

void ConfigObject::setTimeTolerance(float val)
{
	if(val > 1 || val == 0)
	{
		MXT_THROW_EX(ConfigObject::InvalidDataException);
	}
	mTimeTolerance = val;
}

uint16_t ConfigObject::heightSensor(void)
{
	if(!isValid())
	{
		MXT_THROW_EX(ConfigObject::InvalidObjectException);
	}
	return mHeightSensor;
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

float ConfigObject::slowFactor(void)
{
	if(!isValid())
	{
		MXT_THROW_EX(ConfigObject::InvalidObjectException);
	}
	return mSlowFactor;
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
