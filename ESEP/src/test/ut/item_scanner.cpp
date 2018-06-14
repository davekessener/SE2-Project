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
	UNIT_TEST("make a minimal hightmap")
	{
		mItemScanner->takeMeasurement(1);
		MXT_SLEEP(1);
		mItemScanner->takeMeasurement(1);
		MXT_SLEEP(1);
		mItemScanner->takeMeasurement(1);
		MXT_SLEEP(1);
		mItemScanner->takeMeasurement(1);
		MXT_SLEEP(1);
		mItemScanner->takeMeasurement(1);
		MXT_SLEEP(1);
		mItemScanner->takeMeasurement(0);

		MXT_SLEEP(15);
		Data_ptr d = mItemScanner->getHeightmap();

		ASSERT_EQUALS(d->type(), data::DataPoint::Type::HEIGHT_MAP);
		ASSERT_EQUALS(d->to_s(), "HM [6]");
	};
}

}}}
