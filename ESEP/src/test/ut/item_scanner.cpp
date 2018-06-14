#include "test/ut/item_scanner.h"
#include "test/unit/assertions.h"
#include "lib/timer.h"



namespace esep { namespace test { namespace ut {

#define MXT_SLEEP(t)		lib::Timer::instance().sleep(t)

ItemScannerUT::ItemScannerUT()
	: TestSuite("Item Scanner"),
	  mItemScanner(nullptr)
{

}

void ItemScannerUT::setup()
{
	mItemScanner = new ItemScanner(5,10);
}

void ItemScannerUT::teardown()
{
	delete mItemScanner;
	mItemScanner = nullptr;
}

void ItemScannerUT::define()
{
	UNIT_TEST("make a minimal heightmap")
	{
		for (int i = 0; i < 6; i++)
		{
			mItemScanner->takeMeasurement(2);
			MXT_SLEEP(1);
		}

		mItemScanner->takeMeasurement(0);

		MXT_SLEEP(15);
		Data_ptr d = mItemScanner->getHeightmap();

		ASSERT_EQUALS(d->type(), data::DataPoint::Type::HEIGHT_MAP);
		ASSERT_EQUALS(d->to_s(), "HM [6]");
	};

	UNIT_TEST("make a hm too small")
	{
		for (int i = 0; i < 3; i++)
		{
			mItemScanner->takeMeasurement(1);
			MXT_SLEEP(1);
		}
		mItemScanner->takeMeasurement(0);

		MXT_SLEEP(15);
		ASSERT_FAILURE(mItemScanner->getHeightmap(), ItemScanner::NoMeasuredHeighMap);
	};

	UNIT_TEST("taking non existing hm")
	{
		ASSERT_FAILURE(mItemScanner->getHeightmap(), ItemScanner::NoMeasuredHeighMap);
	};

	UNIT_TEST("taking not finished hm")
	{
		for (int i = 0; i < 7; i++)
		{
			mItemScanner->takeMeasurement(4);
			MXT_SLEEP(1);
		}
		mItemScanner->takeMeasurement(0);

		ASSERT_FAILURE(mItemScanner->getHeightmap(), ItemScanner::NoMeasuredHeighMap);

		MXT_SLEEP(15);
		Data_ptr d = mItemScanner->getHeightmap();

		ASSERT_EQUALS(d->type(), data::DataPoint::Type::HEIGHT_MAP);
		ASSERT_EQUALS(d->to_s(), "HM [7]");
	};

	UNIT_TEST("making two hms")
	{
		for (int i = 0; i < 7; i++)
		{
			mItemScanner->takeMeasurement(3);
			MXT_SLEEP(1);
		}
		mItemScanner->takeMeasurement(0);
		MXT_SLEEP(15);

		for (int i = 0; i < 8; i++)
		{
			mItemScanner->takeMeasurement(2);
			MXT_SLEEP(1);
		}
		mItemScanner->takeMeasurement(0);
		MXT_SLEEP(15);

		Data_ptr d = mItemScanner->getHeightmap();

		ASSERT_EQUALS(d->type(), data::DataPoint::Type::HEIGHT_MAP);
		ASSERT_EQUALS(d->to_s(), "HM [7]");

		Data_ptr d2 = mItemScanner->getHeightmap();

		ASSERT_EQUALS(d2->type(), data::DataPoint::Type::HEIGHT_MAP);
		ASSERT_EQUALS(d2->to_s(), "HM [8]");
	};

	UNIT_TEST("making hm too large")
	{
		for (int i = 0; i < 11; i++)
		{
			mItemScanner->takeMeasurement(1);
			MXT_SLEEP(1);
		}
		mItemScanner->takeMeasurement(0);
		MXT_SLEEP(15);

		ASSERT_FAILURE(mItemScanner->getHeightmap(), ItemScanner::NoMeasuredHeighMap);
	};

	UNIT_TEST("suspend while measuring")
	{
		for (int i = 0; i < 3; i++)
		{
			mItemScanner->takeMeasurement(2);
			MXT_SLEEP(1);
		}

		mItemScanner->suspend();

		for (int i = 0; i < 11; i++)
		{
			mItemScanner->takeMeasurement(3);
			MXT_SLEEP(1);
		}

		mItemScanner->resume();

		for (int i = 0; i < 3; i++)
		{
			mItemScanner->takeMeasurement(4);
			MXT_SLEEP(1);
		}
		mItemScanner->takeMeasurement(0);

		MXT_SLEEP(15);

		Data_ptr d2 = mItemScanner->getHeightmap();

		ASSERT_EQUALS(d2->type(), data::DataPoint::Type::HEIGHT_MAP);
		ASSERT_EQUALS(d2->to_s(), "HM [6]");
	};


}

}}}
