/*
 * data_point.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_DATA_DATA_POINT_H
#define SRC_DATA_DATA_POINT_H

#include "lib/byte_stream.h"

namespace esep
{
	namespace data
	{
		class DataPoint
		{
		   public:

			DataPoint();
			virtual ~DataPoint() {}

			virtual void serialize(lib::ByteStream&) = 0;
		};
	}
}



#endif /* SRC_DATA_DATA_POINT_H */
