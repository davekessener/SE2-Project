/*
 * data_point.h
 *
 *  Created on: 26.04.2018
 *      Author: ace991
 */

#ifndef SRC_DATA_DATA_POINT_H_
#define SRC_DATA_DATA_POINT_H_

namespace esep
{
	namespace data
	{
		class Datapoint
		{
		   public:
			Datapoint();
			virtual ~Datapoint();

			virtual void serialize(Bytestream&);

		};
	}
}



#endif /* SRC_DATA_DATA_POINT_H_ */
