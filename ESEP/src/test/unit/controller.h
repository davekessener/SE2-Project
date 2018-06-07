#ifndef ESEP_TEST_UNIT_CONTROLLER_H
#define ESEP_TEST_UNIT_CONTROLLER_H

namespace esep
{
	namespace test
	{
		namespace unit
		{
			class Controller
			{
				public:
					virtual ~Controller( ) { }

					virtual void clear( ) = 0;

				private:
			};
		}
	}
}

#endif
