
#include <iostream>
#include <fstream>
#include <vector>

#include "base/config_object.h"

namespace esep { namespace base {

ConfigObject::ConfigObject(const std::string& path)
	: mPath(path)
	, mHeightSensor(0)
	, mStartToHs(0)
	, mHsToSwitch(0)
	, mSwitchToEnd(0)
	, mSlowFactor(0)
	, mBackwardFactor(0)
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

		if(fileData.size() != 6)
		{
			throw ConfigObject::CurruptDataException();
		}
		else
		{
			mBackwardFactor = std::stof(fileData.back());
			fileData.pop_back();
			mSlowFactor = std::stof(fileData.back());
			fileData.pop_back();
			mSwitchToEnd = std::stoi(fileData.back());
			fileData.pop_back();
			mHsToSwitch = std::stoi(fileData.back());
			fileData.pop_back();
			mStartToHs = std::stoi(fileData.back());
			fileData.pop_back();
			mHeightSensor = (uint16_t) std::stoi(fileData.back());
		}
	}
}

ConfigObject::~ConfigObject(void)
{
	// continue...
}

void ConfigObject::save()
{
	std::ofstream confFile;
	confFile.open(mPath);

	// continue...
}

bool ConfigObject::isValid()
{
	return mHeightSensor > 0
			&& mStartToHs > 0
			&& mHsToSwitch > 0
			&& mSwitchToEnd > 0
			&& mSlowFactor > 0
			&& mBackwardFactor > 0;
}

void ConfigObject::setHeightSensor(uint16_t val)
{
	mHeightSensor = val;
}

void ConfigObject::setStartToHs(uint32_t val)
{
	mStartToHs = val;
}

void ConfigObject::setHsToSwitch(uint32_t val)
{
	mHsToSwitch = val;
}

void ConfigObject::setSwitchToEnd(uint32_t val)
{
	mSwitchToEnd = val;
}

void ConfigObject::setSlowFactor(float val)
{
	if(val > 1)
	{
		throw ConfigObject::CurruptDataException();
	}
	mSlowFactor = val;
}

void ConfigObject::setBackwardFactor(float val)
{
	mBackwardFactor = val;
}

uint16_t ConfigObject::heightSensor(void)
{
	if(!isValid())
	{
		throw ConfigObject::CurruptDataException();
	}
	return mHeightSensor;
}

uint32_t ConfigObject::startToHs(void)
{
	if(!isValid())
	{
		throw ConfigObject::CurruptDataException();
	}
	return mStartToHs;
}

uint32_t ConfigObject::hsToSwitch(void)
{
	if(!isValid())
	{
		throw ConfigObject::CurruptDataException();
	}
	return mHsToSwitch;
}

uint32_t ConfigObject::switchToEnd(void)
{
	if(!isValid())
	{
		throw ConfigObject::CurruptDataException();
	}
	return mSwitchToEnd;
}

float ConfigObject::slowFactor(void)
{
	if(!isValid())
	{
		throw ConfigObject::CurruptDataException();
	}
	return mSlowFactor;
}

float ConfigObject::backwardFactor(void)
{
	if(!isValid())
	{
		throw ConfigObject::CurruptDataException();
	}
	return mBackwardFactor;
}

}}
