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
				static std::shared_ptr<DataPoint> deserialize(lib::ByteStream&)
				{
					MXT_THROW_EX(communication::Packet::NotImplException);
				};
		};

	}
}

#endif /* SRC_DATA_DATA_MANAGER_H_ */
