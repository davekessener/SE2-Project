#include "serial/client/m_reset.h"

#define MXT_RS_1 0x91
#define MXT_RS_2 0x0D
#define MXT_RS_E 0x99

#define MXT_NRS_S 16
#define MXT_NRS_R 8

namespace esep { namespace serial { namespace modules {

void Resetter::send(void)
{
	for(uint i = 0 ; i < MXT_NRS_S ; ++i)
	{
		mConnection.send<byte_t>(MXT_RS_1);
		mConnection.send<byte_t>(MXT_RS_2);
	}

	mConnection.send<byte_t>(MXT_RS_E);
}

void Resetter::receive(void)
{
	uint c = 0;
	byte_t v = 0;
	auto read_rs = [this, &v](void) -> bool {
		v = mConnection.receive<byte_t>();

		if(v == MXT_RS_1)
		{
			v = mConnection.receive<byte_t>();

			if(v == MXT_RS_2)
			{
				return true;
			}
		}

		return false;
	};

	while(c < MXT_NRS_R)
	{
		if(read_rs())
		{
			++c;
		}
		else
		{
			c = 0;
		}
	}

	while(read_rs());

	if(v != MXT_RS_E) handle();
}

}}}
