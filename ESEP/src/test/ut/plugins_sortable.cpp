#include "test/ut/plugins_sortable.h"

#include "master/plugin.h"
#include "master/plugin/sortable.h"

#include "test/unit/assertions.h"

namespace esep { namespace test { namespace ut {

namespace
{
	typedef master::Plugin::Type Type;
	typedef master::Plugin::Action Action;
	typedef std::vector<Type> History;

	template<Type T>
	class TestPlugin : public master::plugin::Sortable<Type::HOLLOW, Type::HOLLOW_METAL, Type::HOLLOW_METAL>
	{
		public:
			TestPlugin( ) : Plugin(T), Sortable(T) { }

			float match(const data_t&) override { return 1.0; }
	};
}

PluginSortable::PluginSortable(void)
	: unit::TestSuite("Plugins Sortable")
{
}

void PluginSortable::define(void)
{
	UNIT_TEST("can create")
	{
		TestPlugin<Type::HOLLOW> p;
	};

	UNIT_TEST("can sort empty")
	{
		TestPlugin<Type::HOLLOW> p;
		History h{};

		ASSERT_EQUALS(p.decide(h), Action::KEEP);
	};

	UNIT_TEST("can sort normal")
	{
		TestPlugin<Type::HOLLOW_METAL> p;
		History h{Type::HOLLOW};

		ASSERT_EQUALS(p.decide(h), Action::KEEP);
	};

	UNIT_TEST("can start in the middle")
	{
		TestPlugin<Type::HOLLOW> p;
		History h{Type::HOLLOW_METAL, Type::HOLLOW_METAL};

		ASSERT_EQUALS(p.decide(h), Action::KEEP);
	};

	UNIT_TEST("can handle long history")
	{
		TestPlugin<Type::HOLLOW> p;
		History h{Type::HOLLOW_METAL, Type::HOLLOW_METAL,
			Type::HOLLOW, Type::HOLLOW_METAL, Type::HOLLOW_METAL,
			Type::HOLLOW, Type::HOLLOW_METAL, Type::HOLLOW_METAL, Type::HOLLOW};

		ASSERT_EQUALS(p.decide(h), Action::KEEP);
	};

	UNIT_TEST("discards items out of order")
	{
		TestPlugin<Type::HOLLOW> p;
		History h{Type::HOLLOW, Type::HOLLOW_METAL};

		ASSERT_EQUALS(p.decide(h), Action::TOSS_S);
	};

	UNIT_TEST("is not impeded by other items")
	{
		TestPlugin<Type::HOLLOW> p1;
		TestPlugin<Type::HOLLOW_METAL> p2;
		History h{Type::CODED_000, Type::FLAT, Type::HOLLOW,
			Type::FLAT, Type::HOLLOW_METAL, Type::CODED_111,
			Type::CODED_010, Type::HOLLOW_METAL, Type::FLAT};

		ASSERT_EQUALS(p1.decide(h), Action::TOSS_S);
		ASSERT_EQUALS(p2.decide(h), Action::KEEP);
	};
}

}}}
