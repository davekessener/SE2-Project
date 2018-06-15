#ifndef SRC_TEST_UT_ITEM_SCANNER_H
#define SRC_TEST_UT_ITEM_SCANNER_H

#include "test/unit/unit.h"

#include "base/run/item_scanner.h"
#include "base/IManager.h"
#include "communication/packet.h"
#include "data/heightmap_data.h"
#include "data/data_point.h"
#include "system.h"
#include "hal.h"


namespace esep
{
	namespace test
	{
		namespace ut
		{

			class ItemScannerUT : public unit::TestSuite
			{
				typedef hal::HAL::Event Event;
				typedef base::run::ItemScanner ItemScanner;
				typedef data::Data_ptr Data_ptr;

				public:
				ItemScannerUT();

				protected:
				   void setup( ) override;
				   void teardown( ) override;
				   void define( ) override;
				private:
				   ItemScanner * mItemScanner;
			};

		}
	}
}


#endif /* SRC_TEST_UT_ITEM_SCANNER_H */
