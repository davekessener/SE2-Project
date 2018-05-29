#ifndef ADC_H_
#define ADC_H_

#include "TSCADC.h"
#include "lib/utils.h"

namespace esep
{
	namespace hal
	{
		namespace adc
		{
			class ADC
			{
				public:
					MXT_DEFINE_E(InterruptAttachException);
					MXT_DEFINE_E(InterruptDetachException);
					MXT_DEFINE_E(NoHwPriviligesException);

				public:
					ADC();
					virtual ~ADC();
					void registerAdcISR(uint, int8_t);
					void unregisterAdcISR(void);
					void adcEnable(unsigned int);
					void adcDisable(void);

				private:
					struct Impl;
					Impl *pImpl;
			};
		}
	}
}

#endif /* ADC_H_ */
