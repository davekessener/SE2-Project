#include "master/plugin.h"

namespace esep { namespace master {

std::string Plugin::action_to_s(Action a)
{
	switch(a)
	{
	case Action::VOID:
		return "VOID";

	case Action::KEEP:
		return "KEEP";

	case Action::TOSS:
		return "TOSS";

	case Action::TOSS_M:
		return "TOSS_M";

	case Action::TOSS_S:
		return "TOSS_S";
	}

	return lib::stringify("ERROR [", a, "]");
}

std::string Plugin::type_to_s(Type t)
{
	switch(t)
	{
	case Type::CODED_000:
		return "CODED (000)";

	case Type::CODED_001:
		return "CODED (001)";

	case Type::CODED_010:
		return "CODED (010)";

	case Type::CODED_011:
		return "CODED (011)";

	case Type::CODED_100:
		return "CODED (100)";

	case Type::CODED_101:
		return "CODED (101)";

	case Type::CODED_110:
		return "CODED (110)";

	case Type::CODED_111:
		return "CODED (111)";

	case Type::FLAT:
		return "FLAT";

	case Type::HOLLOW:
		return "HOLLOW (PLASTIC)";

	case Type::HOLLOW_METAL:
		return "HOLLOW (METAL)";

	case Type::UPSIDEDOWN:
		return "UPSIDE-DOWN";

	case Type::UNKNOWN:
		return "UNKNOWN";

	case Type::DEFAULT:
		return "DEFAULT";
	}

	return lib::stringify("ERROR [", t, "]");
}

}}
