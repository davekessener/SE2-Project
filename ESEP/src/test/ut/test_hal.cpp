#include "test/ut/test_hal.h"

#include "test/unit/assertions.h"

#include "hal.h"

namespace esep { namespace test { namespace ut {

namespace
{
	typedef hal::HAL::Field Field;
	typedef hal::HAL::Event Event;
	typedef hal::Buttons::Button Button;
	typedef hal::LightBarriers::LightBarrier LB;
}

TestHAL::TestHAL(void)
	: unit::TestSuite("Test HAL")
	, mCounter(0)
{
}

void TestHAL::setup(void)
{
	mCounter = 0;
}

void TestHAL::teardown(void)
{
}

void TestHAL::define(void)
{
	UNIT_TEST("can store values")
	{
		hal().setField(Field::GPIO_0, 0x12345678);

		ASSERT_EQUALS(hal().getField(Field::GPIO_0), 0x12345678ul);
	};

	UNIT_TEST("doesn't remember old values")
	{
		ASSERT_NOT_EQUAL(hal().getField(Field::GPIO_0), 0x12345678ul);
	};

	UNIT_TEST("records wrapper usage")
	{
		ASSERT_EQUALS(hal().getField(Field::GPIO_1), 0x00000000ul);

		HAL_MOTOR.start();

		ASSERT_EQUALS(hal().getField(Field::GPIO_1), 0x00001000ul);
	};

	UNIT_TEST("writes a record")
	{
		uint32_t gpio_0 = hal().getField(Field::GPIO_0);

		ASSERT_EQUALS(hal().reads().size(), 0u);
		ASSERT_EQUALS(hal().writes().size(), 0u);

		HAL_MOTOR.start();

		ASSERT_EQUALS(hal().reads().size(), 0u);
		ASSERT_EQUALS(hal().writes().size(), 1u);
		ASSERT_EQUALS(hal().writes().front().get<Field>(), Field::GPIO_1);
		ASSERT_EQUALS(hal().writes().front().get<uint32_t>(), 0x00001000ul);

		HAL_SWITCH.isOpen();

		ASSERT_EQUALS(hal().reads().size(), 1u);
		ASSERT_EQUALS(hal().reads().front().get<Field>(), Field::GPIO_0);
		ASSERT_EQUALS(hal().reads().front().get<uint32_t>(), gpio_0);
	};

	UNIT_TEST("doesn't remember record")
	{
		ASSERT_EQUALS(hal().reads().size(), 0u);
		ASSERT_EQUALS(hal().writes().size(), 0u);
	};

	UNIT_TEST("can register a callback")
	{
		hal().setCallback([this](Event e) { if(e == Event::HEIGHT_SENSOR) ++mCounter; });

		ASSERT_EQUALS(mCounter, 0u);

		hal().trigger(Event::HEIGHT_SENSOR);

		ASSERT_EQUALS(mCounter, 1u);

		hal().trigger(Event::BTN_ESTOP);
		hal().trigger(Event::LB_END);
		hal().trigger(Event::HEIGHT_SENSOR);
		hal().trigger(Event::BTN_RESET);

		ASSERT_EQUALS(mCounter, 2u);
	};

	UNIT_TEST("deletes callback")
	{
		ASSERT_FAILURE(hal().trigger(Event::HEIGHT_SENSOR), unit::Testable::MissingCallbackException);
	};

	UNIT_TEST("initial state is valid")
	{
		ASSERT_TRUE(!HAL_BUTTONS.isPressed(Button::START));
		ASSERT_TRUE(!HAL_BUTTONS.isPressed(Button::STOP));
		ASSERT_TRUE(!HAL_BUTTONS.isPressed(Button::RESET));
		ASSERT_TRUE(!HAL_BUTTONS.isPressed(Button::ESTOP));

		ASSERT_TRUE(!HAL_LIGHT_BARRIERS.isBroken(LB::LB_START));
		ASSERT_TRUE(!HAL_LIGHT_BARRIERS.isBroken(LB::LB_HEIGHTSENSOR));
		ASSERT_TRUE(!HAL_LIGHT_BARRIERS.isBroken(LB::LB_SWITCH));
		ASSERT_TRUE(!HAL_LIGHT_BARRIERS.isBroken(LB::LB_RAMP));
		ASSERT_TRUE(!HAL_LIGHT_BARRIERS.isBroken(LB::LB_END));
	};
}

}}}
