/*
 * data_manager.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_DATA_DATA_MANAGER_H
#define SRC_DATA_DATA_MANAGER_H

#include <memory>
#include "communication/packet.h"

#include "lib/byte_stream.h"
#include "data/data_point.h"

namespace esep
{
	namespace data
	{
		class DataManager
		{
			public:
			MXT_DEFINE_E(UnknownTypeException);
			MXT_DEFINE_E(UnexpectedEOSException);

			public:
				static Data_ptr deserialize(lib::ByteStream&);
		};
	}
}

#endif /* SRC_DATA_DATA_MANAGER_H_ */
