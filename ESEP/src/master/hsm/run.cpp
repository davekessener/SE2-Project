#include "master/hsm/run.h"

#include "lib/logger.h"

#include "data/data_point.h"
#include "data/location_data.h"

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

	typedef data::DataPoint::Type DataType;
	typedef data::Location::Type LocationType;

	inline constexpr event_t to_loc(const LocationType& l) { return static_cast<uint8_t>(l) << 24; }
}

Run::Run(uint c, State *p, IRecipient *m, Analyser *a)
	: Manager(p, m, Message::Base::RUN)
	, mAnalyser(a)
	, mLogic(STATE_COUNT, NIL)
	, mItems(c)
{
	typedef std::function<void(void)> cb_fn;

	auto do_move = [this](uint idx) {
		return [this, idx](void) { mItems.advance(idx); };
	};

	auto do_remove = [this](uint idx) {
		return [this, idx](void) { mItems.remove(idx); };
	};

	auto item_displacement = [this](Location l, LocationType where, Message::Error e) {
		return [this, l, where, e](void) {
			auto p = std::make_shared<Packet>(l, Location::MASTER, e);

			p->addDataPoint(data::Data_ptr(new data::Location(where)));

			mBuffer.push_back(p);
		};
	};

	auto chain = [](cb_fn f1, cb_fn f2) { return [f1, f2](void) { f1(); f2(); }; };

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
	mLogic.transition(Event::fromParts(Location::BASE_M, Message::Run::END_FREE),
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
		chain(
				do_move(S_M2_END),
				[this](void) { mAnalyser->process(mItems.get(S_M2_END)); }));
	mLogic.transition(Event::fromParts(Location::BASE_S, Message::Run::END_FREE),
		{{S_M2_END, 1}},
		{{S_M2RDY, 1}},
		do_move(S_M2_END + 1));

	mLogic.transition(Event::fromParts(Location::BASE_S, Message::Run::NEW_ITEM),
		{{S_M2_PRESTART, 0}},
		{});
//	,item_displacement(Location::BASE_S, LocationType::LB_START, Message::Error::ITEM_APPEARED)); TODO

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

	mLogic.transition(to_loc(LocationType::LB_HEIGHTSENSOR) | Event::fromParts(Location::BASE_M, Message::Run::ITEM_DISAPPEARED),
		{{S_M1_START, 1}, {S_COUNT, 1}},
		{},
		chain(
			item_displacement(Location::BASE_M, LocationType::LB_HEIGHTSENSOR, Message::Error::ITEM_DISAPPEARED),
			do_remove(S_M1_START)));
	mLogic.transition(to_loc(LocationType::LB_SWITCH) | Event::fromParts(Location::BASE_M, Message::Run::ITEM_DISAPPEARED),
		{{S_M1_HS, 1}, {S_COUNT, 1}},
		{},
		chain(
			item_displacement(Location::BASE_M, LocationType::LB_SWITCH, Message::Error::ITEM_DISAPPEARED),
			do_remove(S_M1_HS)));
	mLogic.transition(to_loc(LocationType::LB_END) | Event::fromParts(Location::BASE_M, Message::Run::ITEM_DISAPPEARED),
		{{S_M1_PREEND, 1}, {S_COUNT, 1}},
		{},
		chain(
			item_displacement(Location::BASE_M, LocationType::LB_END, Message::Error::ITEM_DISAPPEARED),
			do_remove(S_M1_PREEND)));

	mLogic.transition(to_loc(LocationType::LB_START) | Event::fromParts(Location::BASE_S, Message::Run::ITEM_DISAPPEARED),
		{{S_M2_PRESTART, 1}, {S_COUNT, 1}},
		{{S_M2RDY, 1}},
		chain(
			item_displacement(Location::BASE_S, LocationType::LB_START, Message::Error::ITEM_DISAPPEARED),
			do_remove(S_M2_PRESTART)));
	mLogic.transition(to_loc(LocationType::LB_HEIGHTSENSOR) | Event::fromParts(Location::BASE_S, Message::Run::ITEM_DISAPPEARED),
		{{S_M2_START, 1}},
		{{S_M2RDY, 1}},
		chain(
			item_displacement(Location::BASE_S, LocationType::LB_HEIGHTSENSOR, Message::Error::ITEM_DISAPPEARED),
			do_remove(S_M2_START)));
	mLogic.transition(to_loc(LocationType::LB_SWITCH) | Event::fromParts(Location::BASE_S, Message::Run::ITEM_DISAPPEARED),
		{{S_M2_HS, 1}},
		{{S_M2RDY, 1}},
		chain(
			item_displacement(Location::BASE_S, LocationType::LB_SWITCH, Message::Error::ITEM_DISAPPEARED),
			do_remove(S_M2_HS)));
	mLogic.transition(to_loc(LocationType::LB_END) | Event::fromParts(Location::BASE_S, Message::Run::ITEM_DISAPPEARED),
		{{S_M2_PREEND, 1}},
		{{S_M2RDY, 1}},
		chain(
			item_displacement(Location::BASE_S, LocationType::LB_END, Message::Error::ITEM_DISAPPEARED),
			do_remove(S_M2_PREEND)));

	mLogic.transition(Event::fromParts(Location::BASE_M, Message::Run::ITEM_REMOVED),
		{{S_M1_SWITCH, 1}, {S_COUNT, 1}},
		{},
		do_remove(S_M1_SWITCH));
	mLogic.transition(Event::fromParts(Location::BASE_S, Message::Run::ITEM_REMOVED),
		{{S_M2_SWITCH, 1}},
		{{S_M2RDY, 1}},
		do_remove(S_M2_SWITCH));

	mLogic.set(S_RUN_RDY, 1);
	mLogic.set(S_M2RDY, 1);


	mLogic.listen(S_RUNNING, [this](void) {
		if(mLogic.get(S_RUNNING))
		{
			send(Location::BASE_M, Message::Run::RESUME);
		}
		else
		{
			send(Location::BASE_M, Message::Run::SUSPEND);
		}
	});

	auto m2_run_logic = [this](void) {
		if(mLogic.get(S_M2RDY) || mLogic.get(S_M2_END))
		{
			send(Location::BASE_S, Message::Run::SUSPEND);
		}
		else
		{
			send(Location::BASE_S, Message::Run::RESUME);
			send(Location::BASE_S, Message::Run::EXPECT_NEW);
		}
	};

	mLogic.listen(S_M2RDY, m2_run_logic);
	mLogic.listen(S_M2_END, m2_run_logic);
}

void Run::enter(void)
{
	Manager::enter();

	if(mLogic.get(S_RUNNING))
	{
		send(Location::BASE_M, Message::Run::RESUME);
	}

	if(!mLogic.get(S_M2RDY))
	{
		send(Location::BASE_S, Message::Run::RESUME);
	}
}

void Run::leave(void)
{
	// SUSPEND ?
}

void Run::handle(Packet_ptr p)
{
	auto e = Event::fromPacket(p);

	for(const auto& d : *p)
	{
		if(d->type() == DataType::LOCATION)
		{
			e |= to_loc(dynamic_cast<data::Location&>(*d).location());
		}
	}

	if(!mLogic.process(e))
	{
//		MXT_LOG_WARN("Received a packet that can't be processed: ", lib::hex<32>(e));
	}

//	MXT_LOG_WARN("### Processing ", lib::hex<32>(e));
//	for(uint i = 0 ; i < STATE_COUNT ; ++i)
//	{
//		MXT_LOG_WARN(i, ": ", mLogic.get(i));
//	}

	if(p->message() == Message::Run::ANALYSE)
	{
		int s = p->source() == Location::BASE_M ? S_M1_SWITCH : S_M2_SWITCH;
		auto& item(mItems.get(s));

		item.location(p->source());

		mAnalyser->analyse(item, Analyser::data_t(p->begin(), p->end()));
		mAnalyser->evaluate(item, history(item.ID()));
	}
	else if(p->message() == Message::Run::ITEM_APPEARED)
	{
		auto pp = std::make_shared<Packet>(*p);
		pp->message(Message::Error::ITEM_APPEARED);
		mBuffer.push_back(pp);
	}

	flush();
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

void Run::flush(void)
{
	for(auto& p : mBuffer)
	{
		com().accept(p);
	}

	mBuffer.clear();
}

}}}
