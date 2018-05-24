/**
 * @file 		ADC.h
 * @brief		This module controls and configures the ADC of the BeagleboneBlack
 * @date 		03.11.2017
 * @author: 	T Jaehnichen
 *				HAW-Hamburg
 *          	Labor f�r technische Informatik
 *          	Berliner Tor  7
 *          	D-20099 Hamburg
 * @version 	1
 * @details
 * @copybrief	Based on the StarterWareFree for AM335X provided by Texas Instrument
 *				
 */

#ifndef ADC_H_
#define ADC_H_

#include "TSCADC.h"

#include "lib/utils.h"

//typedef struct adcInterruptData{
//	struct sigevent* event;
//	TSCADC* tscadc;
//	int isrCoid;
//
//}ADC_INTR_DATA;

namespace esep
{
	namespace hal
	{
		namespace adc
		{
			class ADC{
			public:

				ADC( );
				virtual ~ADC();

				void registerAdcISR(uint, int8_t);
				void unregisterAdcISR(void);
				void adcEnable(unsigned int steps);
				void adcDisable(void);

			private:
				struct Impl;

				Impl *pImpl;
			};
		}
	}
}

#endif /* ADC_H_ */
