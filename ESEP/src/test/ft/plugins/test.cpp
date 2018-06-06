#include "test/ft/plugins.h"

#include "test/ft/plugins/analyser.h"

namespace esep { namespace test { namespace functional {

void testPlugins(const lib::Arguments&)
{
	plugins::Analyser a;

	a.run();
}

}}}
