
#include <string>
#include <fstream>

#include "test/ut/config_object.h"
#include "test/unit/assertions.h"

namespace esep { namespace test { namespace unit {

ConfigObject::ConfigObject(void)
	: TestSuite("Configuration Object")
	, mConfig(nullptr)
{

}

void ConfigObject::setup()
{
	mConfig = new config_t("ut.conf");
}

void ConfigObject::teardown()
{
	delete mConfig;
	remove("ut.conf");
}

void ConfigObject::define()
{

	UNIT_TEST("gives exception if input data is invalid")
	{
		ASSERT_EQUALS(mConfig->isValid(), false);
		ASSERT_FAILURE(mConfig->setHeightSensorMax(0), config_t::InvalidDataException);
		ASSERT_FAILURE(mConfig->setHeightSensorMin(0), config_t::InvalidDataException);
		ASSERT_FAILURE(mConfig->setMaxHandOverTime(0), config_t::InvalidDataException);
		ASSERT_FAILURE(mConfig->setStartToHs(0), config_t::InvalidDataException);
		ASSERT_FAILURE(mConfig->setHsToSwitch(0), config_t::InvalidDataException);
		ASSERT_FAILURE(mConfig->setSwitchToEnd(0), config_t::InvalidDataException);
		ASSERT_FAILURE(mConfig->setSlowFactor(0), config_t::InvalidDataException);
		ASSERT_FAILURE(mConfig->setSlowFactor(1.3), config_t::InvalidDataException);
		ASSERT_FAILURE(mConfig->setSlowFactor(-1), config_t::InvalidDataException);
		ASSERT_FAILURE(mConfig->setTimeTolerance(-1), config_t::InvalidDataException);
		ASSERT_FAILURE(mConfig->setTimeTolerance(1.3), config_t::InvalidDataException);
	};

	UNIT_TEST("object is valid if input data is valid")
	{
		uint16_t sensMax = 12345;
		uint16_t sensMin = 54321;
		uint32_t tim = 123456;
		float slowFact = 0.5;
		float timTol = 0.5;

		ASSERT_EQUALS(mConfig->isValid(), false);

		mConfig->setHeightSensorMax(sensMax);
		mConfig->setHeightSensorMin(sensMin);
		mConfig->setMaxHandOverTime(tim);
		mConfig->setStartToHs(tim);
		mConfig->setHsToSwitch(tim);
		mConfig->setSwitchToEnd(tim);
		mConfig->setSlowFactor(slowFact);

		ASSERT_FAILURE(mConfig->heightSensorMax(), config_t::InvalidObjectException);
		ASSERT_FAILURE(mConfig->heightSensorMin(), config_t::InvalidObjectException);
		ASSERT_FAILURE(mConfig->maxHandOverTime(), config_t::InvalidObjectException);
		ASSERT_FAILURE(mConfig->startToHs(), config_t::InvalidObjectException);
		ASSERT_FAILURE(mConfig->hsToSwitch(), config_t::InvalidObjectException);
		ASSERT_FAILURE(mConfig->switchToEnd(), config_t::InvalidObjectException);
		ASSERT_FAILURE(mConfig->slowFactor(), config_t::InvalidObjectException);

		mConfig->setTimeTolerance(timTol);

		ASSERT_EQUALS(mConfig->isValid(), true);

		ASSERT_EQUALS(mConfig->heightSensorMax(), sensMax);
		ASSERT_EQUALS(mConfig->heightSensorMin(), sensMin);
		ASSERT_EQUALS(mConfig->maxHandOverTime(), tim);
		ASSERT_EQUALS(mConfig->startToHs(), tim);
		ASSERT_EQUALS(mConfig->hsToSwitch(), tim);
		ASSERT_EQUALS(mConfig->switchToEnd(), tim);
		ASSERT_EQUALS(mConfig->slowFactor(), slowFact);
		ASSERT_EQUALS(mConfig->timeTolerance(), timTol);
	};

	UNIT_TEST("can save and open configuration file")
	{
		uint16_t sensMax = 12345;
		uint16_t sensMin = 54321;
		uint32_t tim = 123456;
		float slowFact = 0.5;
		float timTol = 0.5;

		mConfig->setHeightSensorMax(sensMax);
		mConfig->setHeightSensorMin(sensMin);
		mConfig->setMaxHandOverTime(tim);
		mConfig->setStartToHs(tim);
		mConfig->setHsToSwitch(tim);
		mConfig->setSwitchToEnd(tim);
		mConfig->setSlowFactor(slowFact);
		mConfig->setTimeTolerance(timTol);

		mConfig->save();
		delete mConfig;
		mConfig = new config_t("ut.conf");

		ASSERT_EQUALS(mConfig->isValid(), true);
	};

	UNIT_TEST("signal that configuration file is invalid")
		{
			delete mConfig;

			uint16_t sensMax = 12345;
			uint16_t sensMin = 54321;
			uint32_t tim = 123456;
			float timTol = 0.5;

			// saving only 6 parameters to config file
			std::ofstream confFile("ut.conf");
			if(confFile.is_open())
			{
				confFile << timTol << "\n"
						 << sensMin << "\n"
						 << sensMax << "\n"
						 << tim    << "\n"
						 << tim   << "\n"
						 << tim;
			}

			ASSERT_FAILURE(mConfig = new config_t("ut.conf"), config_t::InvalidDataException);
			// just to allow deletion in teardown()
			mConfig = new config_t("ut2.conf");
		};
}

}}}
