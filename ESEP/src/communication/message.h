#ifndef ESEP_COMMUNICATION_MESSAGE_H
#define ESEP_COMMUNICATION_MESSAGE_H

namespace esep
{
	namespace communication
	{
		class Message
		{
			public:

			enum class Master : uint8_t
			{
				CONFIG,
				RUN,
				IDLE,
				READY,
				FIXED
			};

			enum class Base : uint8_t
			{
				CONFIG,
				RUN,
				IDLE,
				READY,
				SHUTDOWN
			};

			enum class Config : uint8_t
			{
				START,
				DONE,
				FAILED
			};

			enum class Run : uint8_t
			{
				NEW_ITEM,
				REACHED_END,
				RAMP_FULL,
				IN_HEIGHTSENSOR,
				ANALYSE,  // analyse item at switch (Base -> Master)
				RESUME, // start motor (Master -> Base)
				SUSPEND, // stop motor (Master -> Base)
				KEEP_NEXT,  // open switch (Master -> Base
				END_FREE,
				EXPECT_NEW,
				ITEM_APPEARED,
				ITEM_DISAPPEARED,
				ITEM_STUCK,
				TIMER,
				ITEM_REMOVED,
				REQUEST_STOP
			};

			enum class Error : uint8_t
			{
				CONFIG,
				SERIAL,
				ITEM_APPEARED,
				ITEM_DISAPPEARED,
				ITEM_STUCK,
				ESTOP,
				RAMP_FULL,
				WARNING
			};

			private:
				Message( ) = delete;
				~Message( ) = delete;
				Message(const Message&) = delete;
				Message& operator=(const Message&) = delete;
		};
	}
}

#endif
