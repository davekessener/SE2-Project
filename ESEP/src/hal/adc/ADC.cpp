
#include <sys/neutrino.h>

#include "ADC.h"
#include "lib/logger.h"

#define ADC_TSC_GENINT 	 	16
#define MXT_BUFSIZE 		50
#define MXT_FREQ 			100
#define MXT_CLOCK 			(MXT_FREQ * MXT_BUFSIZE)

namespace esep { namespace hal { namespace adc {

class ADC::Impl
{
	public:
		Impl( );
		void registerAdcISR(uint, int8_t);
		void unregisterAdcISR(void);
		void adcEnable(unsigned int steps);
		void adcDisable(void);

	private:
		void init(void);
		void initInterrupts(void);
		void stepConfigure(unsigned int stepSel, Fifo fifo, PositiveInput positiveInpChannel);
		void cleanUpInterrupts(void);

		static const struct sigevent* adcISR(void* arg, int id);

	private:
		TSCADC tscadc;
		int interruptID;
		struct sigevent e;
};

const struct sigevent* ADC::Impl::adcISR(void* arg, int id)
{
	static byte_t buf[MXT_BUFSIZE] = { 0 };
	static uint idx = 0;

	ADC::Impl* adc = (ADC::Impl*)arg;

	unsigned int status = adc->tscadc.intStatus();

	adc->tscadc.intStatusClear(status);

	struct sigevent *r = nullptr;

	if(status & END_OF_SEQUENCE_INT)
	{
		buf[idx] = adc->tscadc.fifoADCDataRead(Fifo::FIFO_0) >> 4;

		idx = (idx + 1) % MXT_BUFSIZE;

		if(!idx)
		{
			uint t = 0;

			for(uint i = 0 ; i < MXT_BUFSIZE ; ++i) t += buf[i];

			t /= MXT_BUFSIZE;

			adc->e.sigev_value.sival_int = t;

			r = &adc->e;
		}
	}

	return r;
}

// -----------------------------------------------------------------------------------------------------------------

ADC::ADC(void)
	: pImpl(new Impl)
{
}

ADC::~ADC()
{
	delete pImpl;
}

void ADC::registerAdcISR(uint cid, int8_t c) { pImpl->registerAdcISR(cid, c); }
void ADC::unregisterAdcISR(void) { pImpl->unregisterAdcISR(); }
void ADC::adcEnable(unsigned int steps) { pImpl->adcEnable(steps); }
void ADC::adcDisable(void) { pImpl->adcDisable(); }

// -----------------------------------------------------------------------------------------------------------------

ADC::Impl::Impl(void)
{
	if (-1 == ThreadCtl(_NTO_TCTL_IO, 0))
	{
		MXT_THROW_EX(ADC::NoHwPriviligesException);
	}

	init();
}

void ADC::Impl::registerAdcISR(uint cid, int8_t code)
{
	__asm(" dsb");

	/* Clear the status of all interrupts */
	cleanUpInterrupts();

	SIGEV_PULSE_INIT(&e, cid, SIGEV_PULSE_PRIO_INHERIT, code, 0);

	interruptID = InterruptAttach(ADC_TSC_GENINT, Impl::adcISR, this, sizeof(ADC), 0);
	if(interruptID == -1)
	{
		MXT_THROW_EX(ADC::InterruptAttachException);
	}

	/* Clear the status of all interrupts */
	cleanUpInterrupts();

	/* End of sequence interrupt is enable */
	tscadc.eventInterruptEnable(END_OF_SEQUENCE_INT);

	adcEnable(1);
}

void ADC::Impl::unregisterAdcISR(void)
{
	if( InterruptDetach(interruptID) < 0)
	{
		MXT_THROW_EX(ADC::InterruptDetachException);
	}

	tscadc.eventInterruptDisable(END_OF_SEQUENCE_INT);

	cleanUpInterrupts();
}

void ADC::Impl::init(void)
{
	/* Configures ADC to 3Mhz */
	tscadc.configureAFEClock(24000000, MXT_CLOCK);

	/* Disable Transistor bias */
	tscadc.tsTransistorConfig(false);

	tscadc.stepIDTagConfig(true);

	/* Disable Write Protection of Step Configuration regs*/
	tscadc.stepConfigProtectionDisable();

	/* Configure step 1 for channel 1(AN0)*/
	stepConfigure(0, Fifo::FIFO_0, POSITIVE_INP_CHANNEL1);

	/* General purpose inputs */
	tscadc.tsModeConfig(GENERAL_PURPOSE_MODE);
	tscadc.fifoIRQThresholdLevelConfig(Fifo::FIFO_0, 1);

	/* Clear the status of all interrupts */
	cleanUpInterrupts();

	/* End of sequence interrupt is enable */
	tscadc.eventInterruptEnable(END_OF_SEQUENCE_INT);

}

void ADC::Impl::stepConfigure(unsigned int stepSel, Fifo fifo, PositiveInput positiveInpChannel)
{
    /* Configure ADC to Single ended operation mode */
    tscadc.tsStepOperationModeControl(SINGLE_ENDED_OPER_MODE, stepSel);

    /* Configure step to select Channel, refernce voltages */
    tscadc.tsStepConfig(stepSel, NEGATIVE_REF_VSSA,
                    positiveInpChannel, NEGATIVE_INP_CHANNEL1, POSITIVE_REF_VDDA);

    /* XPPSW Pin is on, Which pull up the AN0 line*/
    tscadc.tsStepAnalogSupplyConfig(true, false, false, stepSel);

    /* XNNSW Pin is on, Which pull down the AN1 line*/
    tscadc.tsStepAnalogGroundConfig(true, false, false, false, stepSel);

    /* select fifo 0 or 1*/
    tscadc.tsStepFIFOSelConfig(stepSel, fifo);

    /* Configure ADC to one shot mode */
    tscadc.tsStepModeConfig(stepSel,  CONTINIOUS_SOFTWARE_ENABLED);

    tscadc.tsStepAverageConfig(stepSel, NO_SAMPLES_AVG);
}

/**
 * @brief	This function enables the given number of steps.
 *
 * @param 	steps	Number between 1 and 16 which shall be enabled.
 *
 * @return 	None
 */
void ADC::Impl::adcEnable(unsigned int steps)
{
	tscadc.moduleStateSet(false);
	int maxSteps = steps<16?steps:16;
	for(int i=1; i<=maxSteps; i++)
	{
		tscadc.configureStepEnable(i, true);
	}
	tscadc.moduleStateSet(true);
}

void ADC::Impl::adcDisable(void)
{
	tscadc.moduleStateSet(false);
}

void ADC::Impl::cleanUpInterrupts(void)
{
	tscadc.intStatusClear(0x7FF);
	tscadc.intStatusClear(0x7FF);
	tscadc.intStatusClear(0x7FF);
}

}}}
