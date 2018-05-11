#ifndef ESEP_LIB_READER_H
#define ESEP_LIB_READER_H

#include <string>
#include <memory>

#include "lib/utils.h"

namespace esep
{
	namespace lib
	{
		class Reader
		{
			public:
			MXT_DEFINE_E(ReaderEmptyException);

			public:
				virtual ~Reader( ) { }
				std::string readLine( ) { if(empty()) MXT_THROW_EX(ReaderEmptyException); return doReadLine(); }
				virtual bool empty( ) const = 0;

			protected:
				virtual std::string doReadLine( ) = 0;

			private:
		};

		typedef std::unique_ptr<Reader> Reader_ptr;
	}
}

#endif
