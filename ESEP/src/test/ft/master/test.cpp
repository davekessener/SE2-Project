#include "test/ft/master.h"

#include "test/ft/master/system.h"

#include "lib/console.h"

#include "lib/sync/container.h"

namespace esep { namespace test { namespace functional {

class ConsoleReader : public lib::Reader
{
	public:
		ConsoleReader( ) { lib::Console::instance().setCallback([this](const std::string& s) { mBuffer.insert(s); }); }

		bool empty( ) const override { return false; }

	protected:
		std::string doReadLine( ) override { return mBuffer.remove(); }

	private:
		sync::Container<std::string> mBuffer;
};

class ConsoleWriter : public lib::Writer
{
	public:
		void writeLine(const std::string& line) override { lib::Console::instance().println(line); }
};

void testMasterLogic(const lib::Arguments&)
{
	m::System sys(lib::Reader_ptr(new ConsoleReader), lib::Writer_ptr(new ConsoleWriter));

	sys.run();
}


}}}
