#include "test/ft/runmanager.h"

#include "test/ft/runmanager/tester.h"

namespace esep { namespace test { namespace functional {

void testRunManager(const lib::Arguments&)
{
	r::Tester t;

	t.run();
}

}}}
