/*
 * IManager.h
 *

 */

#ifndef SRC_BASE_IMANAGER_H
#define SRC_BASE_IMANAGER_H

#include "communication/IRecipient.h"

namespace esep
{
	namespace base
	{
		class IManager : public IRecipient
		{
			public:
			   IManager();
			   //Ich moechte eigentlich das ein Error auftritt, wenn der Funktion nicht vom erbenden implementiert wird. So wird er ja nicht gezwungen diese zu implementieren
			   virtual ~IManager()
			   {};
			   virtual void enter()
			   {};
			   virtual void leave()
			   {};
			   virtual void handle(hal::HAL::Event)
			   {};

		};

}
}



#endif /* SRC_BASE_IMANAGER_H_ */
