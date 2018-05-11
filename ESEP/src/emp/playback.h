#ifndef ESEP_EMP_PLAYBACK_H
#define ESEP_EMP_PLAYBACK_H

#include <vector>
#include <array>
#include <atomic>

#include "qnx/connection.h"

#include "hal/hal.h"
#include "hal/buffer.h"

#include "lib/utils.h"
#include "lib/reader.h"
#include "lib/timer.h"

namespace esep
{
	namespace emp
	{
		class Playback : public hal::Buffer
		{
			typedef lib::Reader_ptr Reader_ptr;
			typedef hal::HAL::Field Field;
			typedef uint64_t entry_t;
			typedef std::vector<entry_t> entries_t;
			typedef entries_t::const_iterator iter_t;

			public:
			MXT_DEFINE_E(InvalidFileException);

			public:
				Playback(Reader_ptr);
				void out(Field, uint32_t) override;
				void set(Field f, bitmask_t v) override { out(f, in(f) | v); }
				void reset(Field f, bitmask_t v) override { out(f, in(f) & ~v); }

			private:
				void add(entry_t);
				void next( );
				void execute( );

			private:
				entries_t mEntries;
				iter_t mCurrent, mEnd;
				timer::Manager mTimer;
		};
	}
}

#endif
