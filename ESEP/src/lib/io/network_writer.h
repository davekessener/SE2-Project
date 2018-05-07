#ifndef ESEP_LIB_NETWORKWRITER_H
#define ESEP_LIB_NETWORKWRITER_H

#include "lib/writer.h"
#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		class NetworkWriter : public Writer
		{
			public:
			MXT_DEFINE_E(ConnectionFailedException);

			public:
				NetworkWriter(const std::string&, uint);
				virtual ~NetworkWriter( );
				void write(const std::string&) override;
			private:
				struct Impl;

				Impl *pImpl;
		};
	}
}

#endif
