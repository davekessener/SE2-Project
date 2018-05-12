#ifndef ESEP_TEST_UNIT_COMPOUNDENUM_H
#define ESEP_TEST_UNIT_COMPOUNDENUM_H

#include "test/unit/unit.h"

#include "lib/enum.h"

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class CompoundEnum : public TestSuite
			{
				enum class Small : uint8_t
				{
					A,
					B,
					C,
					D = 255
				};

				enum class Large : int16_t
				{
					A,
					B,
					E,
					F = -1,
					G = 256
				};

				enum class Other : int8_t
				{
					D = -1
				};

				typedef tml::MakeCompoundEnum<Small, Large, Other> enum_t;

				public:
					CompoundEnum( );
				protected:
					void define( ) override;
				private:
			};
		}
	}
}

#endif
