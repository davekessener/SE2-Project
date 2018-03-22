#include "hal/recorder.h"

#define MXT_DEFAULT_FLAGS (std::ios::showbase|std::ios::internal|std::ios::hex)
#define MXT_DEFAULT_PRECISION (0)
#define MXT_DEFAULT_WIDTH (2)
#define MXT_DEFAULT_FILL ('0')

namespace esep { namespace hal {

typedef lib::IOStreamFormatBackup<std::ostream> IOSBackup;

Recorder::Recorder(HAL *hal, std::ostream& os)
	: mWrappedHAL(hal)
	, mOS(os)
	, mIOFormat(MXT_DEFAULT_FLAGS, MXT_DEFAULT_PRECISION, MXT_DEFAULT_WIDTH, MXT_DEFAULT_FILL)
{
}

uint8_t Recorder::in8(Port p)
{
	uint8_t v = mWrappedHAL->in8(p);

	IOSBackup backup(mOS);

	mIOFormat.apply(mOS);

	// TODO actual logging protocol
	mOS << "Read " << v << " from port " << static_cast<uint8_t>(p) << "\n";

	return v;
}

uint16_t Recorder::in16(void)
{
	uint16_t v = mWrappedHAL->in16();

	IOSBackup backup(mOS);

	mIOFormat.apply(mOS);
	mOS.width(4);

	// TODO actual loggin protocol
	mOS << "Read " << v << " from analog.\n";

	return v;
}

void Recorder::out8(Port p, uint8_t v)
{
	mWrappedHAL->out8(p, v);

	IOSBackup backup(mOS);

	mIOFormat.apply(mOS);

	// TODO actual logging protocol
	mOS << "Wrote " << v << " to port " << static_cast<uint8_t>(p) << "\n";
}

}}

