#include "lib/log/format.h"

namespace esep { namespace log { namespace format {

namespace
{
	inline constexpr uint offsetInt(char c) { return (c >= '0' && c <= '9') ? 1 + c - '0' : 0; }
}

void Parser::parse(char c)
{
	switch(mState)
	{
		case State::IDLE:
			mState = onIdle(c);
			break;
		case State::ANTICIPATING:
			mState = onAnticipating(c);
			break;
		case State::SPACING:
			mState = onSpacing(c);
			break;
		case State::PROCESSING:
			mState = onProcessing(c);
			break;
	}

	mBak << c;
}

Parser::State Parser::onIdle(char c)
{
	if(c == '%')
	{
		mSpacing = 0;

		return State::ANTICIPATING;
	}
	else if(c == '\0')
	{
		push();

		return State::IDLE;
	}
	else
	{
		mSS << c;

		return State::IDLE;
	}
}

Parser::State Parser::onAnticipating(char c)
{
	if(c == '\0')
		throw lib::stringify("Not properly terminated format string in '", mBak.str(), "'!");

	mID.str("");
	mSpacing = 0;

	if(c == '%')
	{
		mSS << '%';

		return State::IDLE;
	}
	else if(uint v = offsetInt(c))
	{
		mSpacing = v - 1;

		push();

		return State::SPACING;
	}
	else
	{
		mID << c;

		push();

		return State::PROCESSING;
	}
}

Parser::State Parser::onSpacing(char c)
{
	if(c == '\0')
		throw lib::stringify("Not properly terminated format string in '", mBak.str(), "'!");

	if(uint v = offsetInt(c))
	{
		mSpacing = mSpacing * 10 + (v - 1);

		return State::SPACING;
	}
	else
	{
		mID << c;

		return State::PROCESSING;
	}
}

Parser::State Parser::onProcessing(char c)
{
	if(c == '\0')
		throw lib::stringify("Not properly terminated format string in '", mBak.str(), "'!");

	if(c != '%')
	{
		mID << c;

		return State::PROCESSING;
	}
	else
	{
		std::string s(mID.str());

		for(uint i = 0 ; i < mN ; ++i)
		{
			if(s == mTable[i])
			{
				format.push_back(std::make_pair(i, mSpacing));

				return State::IDLE;
			}
		}

		throw lib::stringify("Unknown ID '", s, "' in format string '", mBak.str(), "'!");
	}
}

void Parser::push(void)
{
	std::string s(mSS.str());

	if(!s.empty())
	{
		literals.push_back(s);
		format.push_back(std::make_pair(-1, 0));
	}

	mSS.str("");
}

}}}
