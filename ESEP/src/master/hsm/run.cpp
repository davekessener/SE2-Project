#include "master/hsm/run.h"

namespace esep { namespace master { namespace hsm {

namespace
{
	enum
	{
		S_M1_START,
		S_M1_HS,
		S_M1_SWITCH,
		S_M1_PREEND,
		S_M1_END,
		S_M2_START,
		S_M2_HS,
		S_M2_SWITCH,
		S_M2_PREEND,
		S_M2_END,

		S_M2_PRESTART,
		S_M2RDY,
		S_COUNT,
		S_RUN_RDY,
		S_RUNNING,
		S_RUN_PAUSED,
		STATE_COUNT
	};

	constexpr event_t NIL = -1;
}

Run::Run(uint c, State *p, Sender *m, Analyser *a)
	: Manager(p, m, Message::Base::RUN)
	, mAnalyser(a)
	, mLogic(STATE_COUNT, NIL)
	, mItems(c)
{
	auto do_move = [this](uint idx) {
		return [this, idx](void) { mItems.advance(idx); };
	};

	mLogic.transition(Event::fromParts(Location::BASE_M, Message::Run::NEW_ITEM),
		{},
		{{S_M1_START, 1}, {S_COUNT, 1}},
		do_move(S_M1_START));
	mLogic.transition(Event::fromParts(Location::BASE_M, Message::Run::IN_HEIGHTSENSOR),
		{{S_M1_START, 1}},
		{{S_M1_HS, 1}},
		do_move(S_M1_HS));
	mLogic.transition(Event::fromParts(Location::BASE_M, Message::Run::ANALYSE),
		{{S_M1_HS, 1}},
		{{S_M1_SWITCH, 1}},
		do_move(S_M1_SWITCH));
	mLogic.transition(Event::fromParts(Location::BASE_M, Message::Run::KEEP_NEXT),
		{{S_M1_SWITCH, 1}},
		{{S_M1_PREEND, 1}},
		do_move(S_M1_PREEND));
	mLogic.transition(Event::fromParts(Location::BASE_M, Message::Run::REACHED_END),
		{{S_M1_PREEND, 1}},
		{{S_M1_END, 1}},
		do_move(S_M1_END));

	mLogic.transition(Event::fromParts(Location::BASE_S, Message::Run::NEW_ITEM),
		{{S_M2_PRESTART, 1}, {S_COUNT, 1}},
		{{S_M2_START, 1}},
		do_move(S_M2_START));
	mLogic.transition(Event::fromParts(Location::BASE_S, Message::Run::IN_HEIGHTSENSOR),
		{{S_M2_START, 1}},
		{{S_M2_HS, 1}},
		do_move(S_M2_HS));
	mLogic.transition(Event::fromParts(Location::BASE_S, Message::Run::ANALYSE),
		{{S_M2_HS, 1}},
		{{S_M2_SWITCH, 1}},
		do_move(S_M2_SWITCH));
	mLogic.transition(Event::fromParts(Location::BASE_S, Message::Run::KEEP_NEXT),
		{{S_M2_SWITCH, 1}},
		{{S_M2_PREEND, 1}},
		do_move(S_M2_PREEND));
	mLogic.transition(Event::fromParts(Location::BASE_S, Message::Run::REACHED_END),
		{{S_M2_PREEND, 1}},
		{{S_M2_END, 1}},
		do_move(S_M2_END));
	mLogic.transition(Event::fromParts(Location::BASE_S, Message::Run::END_FREE),
		{{S_M2_END, 1}},
		{{S_M2RDY, 1}},
		do_move(S_M2_END + 1));

	mLogic.transition(Event::fromParts(Location::BASE_S, Message::Run::NEW_ITEM),
		{{S_M2_PRESTART, 0}},
		{},
		[this](void) { com().send(Location::MASTER, Message::Error::ITEM_APPEARED); }); // TODO DataPoint location

	mLogic.transition(NIL,
		{{S_M1_END, 1}, {S_M2RDY, 1}, {S_RUN_PAUSED, 0}},
		{{S_M2_PRESTART, 1}});
	mLogic.transition(NIL,
		{{S_COUNT, 1}, {S_RUN_RDY, 1}},
		{{S_COUNT, 1}, {S_RUNNING, 1}});
	mLogic.transition(NIL,
		{{S_RUNNING, 1}, {S_COUNT, 0}},
		{{S_RUN_RDY, 1}});
	mLogic.transition(NIL,
		{{S_RUNNING, 1}, {S_M1_END, 1}, {S_M2RDY, 0}},
		{{S_M1_END, 1}, {S_RUN_PAUSED, 1}});
	mLogic.transition(NIL,
		{{S_RUN_PAUSED, 1}, {S_M2RDY, 1}},
		{{S_M2RDY, 1}, {S_RUNNING, 1}});

	mLogic.set(S_RUN_RDY, 1);
	mLogic.set(S_M2RDY, 1);
}

void Run::enter(void)
{
	Manager::enter();

	if(mLogic.get(S_RUNNING))
	{
		com().send(Location::BASE_M, Message::Run::RESUME);
	}

	if(!mLogic.get(S_M2RDY))
	{
		com().send(Location::BASE_S, Message::Run::RESUME);
	}
}

void Run::leave(void)
{
	// SUSPEND ?
}

void Run::handle(Packet_ptr p)
{
	mLogic.process(Event::fromPacket(p));

	if(p->message() == Message::Run::ANALYSE)
	{
		int s = p->source() == Location::BASE_M ? S_M1_SWITCH : S_M2_SWITCH;
		auto& item(mItems.get(s));

		item.location(p->source());

		mAnalyser->analyse(item, Analyser::data_t(p->begin(), p->end()));
		mAnalyser->evaluate(item, history(item.ID()));
	}
}

Run::history_t Run::history(id_t id)
{
	history_t h;
	auto i1 = mItems.begin(), i2 = mItems.end();

	for(; i1 != i2 ; ++i1)
	{
		if(i1->ID() == id) break;
	}

	if(i1 != i2)
	{
		++i1;

		for(; i1 != i2 ; ++i1)
		{
			h.push_back(i1->plugin() ? i1->plugin()->type() : Plugin::Type::UNKNOWN);
		}
	}

	return h;
}

}}}
