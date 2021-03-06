#include "master/logic.h"

#include "communication/message.h"

#include "master/hsm/error.h"
#include "master/hsm/manager.h"
#include "master/hsm/forward.h"

#include "lib/logger.h"

#define MXT_MAX_DEPTH 500

namespace esep { namespace master {

Logic::Logic(IRecipient *s, Analyser *a)
{
	typedef lib::hsm::Machine<hsm::Base> Machine;
	typedef lib::hsm::Leaf<hsm::Base> Leaf;
	typedef std::unique_ptr<State> State_ptr;
	typedef communication::Message Message;

	mRampFull[0] = mRampFull[1] = false;

	lib::hsm::Builder<event_t, hsm::Base> builder;

	State *root = new Machine(false, nullptr);
	State *working = new Machine(true, root);
	State *error = new Machine(false, root);

	State *error_both = new hsm::Error(error, s);
	State *error_m = new Leaf(error);
	State *error_s = new Leaf(error);

	State *idle = new hsm::Manager(working, s, Message::Base::IDLE);
	State *valid = new hsm::Manager(working, s, Message::Base::VALID);
	State *ready = new hsm::Manager(working, s, Message::Base::READY);
	State *config = new hsm::Manager(working, s, Message::Base::CONFIG);
	mRun = new hsm::Run(MXT_MAX_DEPTH, working, s, a);
	State *run = mRun;

	State *run_both_empty = new hsm::Forward(run, run);
	State *run_m_empty = new hsm::Forward(run, run);
	State *run_s_empty = new hsm::Forward(run, run);

	State *config_m1 = new Machine(false, config);
	State *config_m2 = new Machine(false, config);

	State *config_m1_rdy = new Leaf(config_m1);
	State *config_m1_running = new Leaf(config_m1);

	State *config_m2_rdy = new Leaf(config_m2);
	State *config_m2_running = new Leaf(config_m2);

	State *idle_either = new Leaf(idle);
	State *idle_m1 = new Leaf(idle);
	State *idle_m2 = new Leaf(idle);

	builder.add(State_ptr(root), State_ptr(working), State_ptr(error),
		State_ptr(error_both), State_ptr(error_m), State_ptr(error_s),
		State_ptr(idle), State_ptr(ready), State_ptr(config), State_ptr(run), State_ptr(valid),
		State_ptr(run_both_empty), State_ptr(run_m_empty), State_ptr(run_s_empty),
		State_ptr(idle_either), State_ptr(idle_m1), State_ptr(idle_m2),
		State_ptr(config_m1), State_ptr(config_m1_rdy), State_ptr(config_m1_running),
		State_ptr(config_m2), State_ptr(config_m2_rdy), State_ptr(config_m2_running));

	builder.transition(working, error_both, Event::ERROR);
	builder.transition(error_m, error_both, Event::ERROR);
	builder.transition(error_s, error_both, Event::ERROR);
	builder.transition(error_both, error_m, Event::fromParts(Location::BASE_S, Message::Master::FIXED));
	builder.transition(error_both, error_s, Event::fromParts(Location::BASE_M, Message::Master::FIXED));
	builder.transition(error_m, ready, Event::fromParts(Location::BASE_M, Message::Master::FIXED));
	builder.transition(error_s, ready, Event::fromParts(Location::BASE_S, Message::Master::FIXED));

	builder.transition(idle, config_m1_rdy, Event::fromParts(Location::BASE_M, Message::Master::CONFIG));
	builder.transition(idle, config_m1_rdy, Event::fromParts(Location::BASE_S, Message::Master::CONFIG));
	builder.transition(valid, config_m1_rdy, Event::fromParts(Location::BASE_M, Message::Master::CONFIG));
	builder.transition(valid, config_m1_rdy, Event::fromParts(Location::BASE_S, Message::Master::CONFIG));
	builder.transition(idle_either, idle_m2, Event::fromParts(Location::BASE_M, Message::Master::VALID));
	builder.transition(idle_either, idle_m1, Event::fromParts(Location::BASE_S, Message::Master::VALID));
	builder.transition(idle_m1, valid, Event::fromParts(Location::BASE_M, Message::Master::VALID));
	builder.transition(idle_m2, valid, Event::fromParts(Location::BASE_S, Message::Master::VALID));

	builder.transition(config_m1_rdy, config_m1_running, Event::fromParts(Location::BASE_M, Message::Config::START));
	builder.transition(config_m1_running, config_m2_rdy, Event::fromParts(Location::BASE_M, Message::Config::DONE),
		[this, s](State&, State&, event_t) { s->accept(std::make_shared<Packet>(Location::MASTER, Location::BASE, Message::Run::RESUME)); });
	builder.transition(config_m2_rdy, config_m2_running, Event::fromParts(Location::BASE_S, Message::Config::START),
		[this, s](State&, State&, event_t) { s->accept(std::make_shared<Packet>(Location::MASTER, Location::BASE_M, Message::Run::SUSPEND)); });
	builder.transition(config_m2_running, valid, Event::fromParts(Location::BASE_S, Message::Config::DONE));

	builder.transition(valid, run, Event::fromParts(Location::BASE_M, Message::Master::RUN));
	builder.transition(valid, run, Event::fromParts(Location::BASE_S, Message::Master::RUN));

	builder.transition(ready, run, Event::fromParts(Location::BASE_M, Message::Master::RUN));
	builder.transition(ready, run, Event::fromParts(Location::BASE_S, Message::Master::RUN));

	builder.transition(run, ready, Event::fromParts(Location::BASE_M, Message::Master::READY));
	builder.transition(run, ready, Event::fromParts(Location::BASE_S, Message::Master::READY));
	builder.transition(run, valid, Event::fromParts(Location::BASE_M, Message::Master::IDLE));
	builder.transition(run, valid, Event::fromParts(Location::BASE_S, Message::Master::IDLE));

	builder.transition(run_both_empty, run_m_empty, Event::fromParts(Location::BASE_S, Message::Run::RAMP_FULL),
			[this](State&, State&, event_t) { mRampFull[1] = true; });
	builder.transition(run_both_empty, run_s_empty, Event::fromParts(Location::BASE_M, Message::Run::RAMP_FULL),
			[this](State&, State&, event_t) { mRampFull[0] = true; });
	builder.transition(run_m_empty, run_both_empty, Event::fromParts(Location::BASE_M, Message::Run::RAMP_FULL),
			[this, s](State&, State&, event_t) {
				mRampFull[1] = false;
				s->accept(std::make_shared<Packet>(Location::MASTER, Location::MASTER, Message::Error::RAMP_FULL));
			});
	builder.transition(run_s_empty, run_both_empty, Event::fromParts(Location::BASE_S, Message::Run::RAMP_FULL),
			[this, s](State&, State&, event_t) {
				mRampFull[0] = false;
				s->accept(std::make_shared<Packet>(Location::MASTER, Location::MASTER, Message::Error::RAMP_FULL));
			});

	root->initial(working);
	working->initial(idle);
	idle->initial(idle_either);
	config->initial(config_m1);
	config_m1->initial(config_m1_rdy);
	run->initial(run_both_empty);
	error->initial(error_both);

	builder.initial(idle_either);

	mLogic = builder.build();
}

void Logic::process(event_t e)
{
	mLogic.process(e);
}

void Logic::accept(Packet_ptr p)
{
	mLogic.getActive()->handle(p);
}

}}
