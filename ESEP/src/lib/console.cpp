#include <iostream>

#include "lib/console.h"

#include "lib/logger.h"

namespace esep { namespace lib { namespace console {

Impl::Impl(void)
{
	mReader.construct([this](void) {
		try
		{
			for(std::string line ; std::getline(std::cin, line) ;)
			{
				mCallback([&](callback_fn& f) {
					if(static_cast<bool>(f))
					{
						f(line);
					}
				});
			}
		}
		MXT_CATCH_ALL_STRAY
	});
}

Impl::~Impl(void)
{
	mCallback([](callback_fn& f) { f = nullptr; });
	mReader.detach();
}

void Impl::doPrint(const std::string& s)
{
	MXT_LOG_INFO("'", s, "'");

	MXT_SYNCHRONIZE;

	std::cout << s << std::flush;
}

void Impl::setCallback(callback_fn f)
{
	mCallback([&](callback_fn& cb) {
		if(static_cast<bool>(cb))
		{
			MXT_LOG_WARN("Overriding console callback!");
		}

		cb = f;
	});
}

}}}
