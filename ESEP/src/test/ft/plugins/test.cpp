#include "test/ft/plugins.h"

#include "test/ft/plugins/analyser.h"
#include "test/ft/plugins/demo.h"

namespace esep { namespace test { namespace functional {

void testPlugins(const lib::Arguments&)
{
//	plugins::Analyser a;
//
//	a.run();

	plugins::Demo d;

	d.run();
}

}}}
