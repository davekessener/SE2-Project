#include <iostream>

#include "system.h"

int main(int argc, char *argv[])
try
{
	esep::System::instance().run();

	return 0;
}
catch(const std::string& e)
{
	std::cerr << "ERROR: Uncaught exception!\n" << e << std::endl;
}
