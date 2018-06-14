#include "emp/playback.h"

#include "emp/location.h"
#include "emp/tokenizer.h"
#include "emp/parser.h"

#include "lib/logger.h"

#define MXT_1MS_IN_NS 1000000

namespace esep { namespace emp {

typedef uint64_t entry_t;

namespace
{
	enum class Code : int8_t
	{
		SHUTDOWN,
		TICK
	};

	inline constexpr uint get_time(const entry_t e)             { return e & 0xFFFFFFFF; }
	inline constexpr uint get_value(const entry_t e)            { return (e >> 32) & 0xFFFF; }
	inline constexpr uint32_t get_pin(const entry_t e)          { return 1 << ((e >> 48) & 0x1F); }
	inline constexpr hal::HAL::Field get_field(const entry_t e) { return static_cast<hal::HAL::Field>((e >> 56) & 0x3); }

	inline constexpr entry_t set_time(const entry_t e, const uint32_t t) { return (e & 0xFFFFFFFF00000000ull) | t; }

	entry_t assemble(uint32_t t, const Location& l, uint32_t v)
	{
		uint p = l.pin;

		if(l.field != hal::HAL::Field::ANALOG)
		{
			for(uint i = 0 ; i < sizeof(v) * 8 ; ++i)
			{
				if(p & (1 << i))
				{
					p = i;
					break;
				}
			}
		}

		return (static_cast<uint64_t>(l.field) << 56) | ((p & 0x1Full) << 48) | ((v & 0xFFFFull) << 32) | t;
	}
}

// # ------------------------------------------------------------------------

Playback::Playback(Reader_ptr in, hal::HAL_ptr hal)
	: mHAL(std::move(hal))
{
	update(Field::GPIO_0,
		  HAL::getPin(Event::LB_START)
		| HAL::getPin(Event::LB_HEIGHTSENSOR)
		| HAL::getPin(Event::LB_SWITCH)
		| HAL::getPin(Event::LB_RAMP)
		| HAL::getPin(Event::LB_END)
		| HAL::getPin(Event::BTN_STOP)
		| HAL::getPin(Event::BTN_ESTOP)
	);
	update(Field::ANALOG, 0x00000000);

	while(!in->empty())
	{
		std::vector<std::string> buf;

		Tokenizer::tokenize(buf, in->readLine());

		if(buf.empty()) continue;

		if(buf.size() != 3)
		{
			MXT_THROW_EX(InvalidFileException);
		}

		auto t = Parser::parseTime(buf[0]);
		auto l = Parser::parseSensor(buf[1]);
		auto v = Parser::parseValue(buf[2]);

		add(assemble(t, l, v));
	}

	mCurrent = mEntries.cbegin();
	mEnd = mEntries.cend();

	next();

	MXT_LOG_WARN("EMP was initialized with an event @0ms. This may not work!");
}

void Playback::out(Field f, uint32_t v)
{
	if(static_cast<bool>(mHAL))
	{
		mHAL->out(f, v);
	}
}

void Playback::set(Field f, uint32_t v)
{
	if(static_cast<bool>(mHAL))
	{
		mHAL->set(f, v);
	}
}

void Playback::reset(Field f, uint32_t v)
{
	if(static_cast<bool>(mHAL))
	{
		mHAL->reset(f, v);
	}
}

void Playback::add(entry_t e)
{
	auto t = get_time(e);

	for(auto i1 = mEntries.begin(), i2 = mEntries.end() ; i1 != i2 ; ++i1)
	{
		if(get_time(*i1) >= t)
		{
			mEntries.insert(i1, e);

			return;
		}
	}

	mEntries.push_back(e);
}

void Playback::next(void)
{
	if(mCurrent != mEnd)
	{
		auto f = [this](void) {
			execute();
			++mCurrent;
			next();
		};

		auto t = lib::Timer::instance().elapsed();

		if(t < get_time(*mCurrent))
		{
			mTimer = lib::Timer::instance().registerCallback(f, get_time(*mCurrent) - t);
		}
		else
		{
			f();
		}
	}
}

void Playback::execute(void)
{
	auto& e(*mCurrent);

	auto f = get_field(e);
	auto v = get_value(e);
	auto p = get_pin(e);

	if(f == Field::ANALOG)
	{
		update(Field::ANALOG, v);
	}
	else
	{
		if(v)
		{
			update(f, in(f) | p);
		}
		else
		{
			update(f, in(f) & ~p);
		}
	}
}

}}
