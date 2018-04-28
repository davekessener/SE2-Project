/*
 * IManager.h
 *

 */

#ifndef SRC_BASE_IMANAGER_H_
#define SRC_BASE_IMANAGER_H_

namespace esep
{
	namespace base
	{
		class IManager : public IRecipient
		{
			public:
			   IManager();
			   virtual ~IManager();
			   virtual void enter();
			   virtual void leave();

		};
}
}



#endif /* SRC_BASE_IMANAGER_H_ */
