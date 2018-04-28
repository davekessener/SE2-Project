/*
 * data_manager.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_DATA_DATA_MANAGER_H_
#define SRC_DATA_DATA_MANAGER_H_

#include <memory>

#include "lib/byte_stream.h"
#include "data/data_point.h"
namespace esep
{
	namespace data
	{
		class Datamanager
		{
			public:

				Datamanager();
				virtual ~Datamanager();
				static std::shared_ptr<DataPoint> deserialize(lib::ByteStream&);

			private:

		};

	}
}




#endif /* SRC_DATA_DATA_MANAGER_H_ */
