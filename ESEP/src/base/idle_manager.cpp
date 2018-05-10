
#include "idle_manager.h"


namespace esep{ namespace base{


IdleManager::IdleManager(communication::IRecipient *basehandler)
	:mBaseHandler(basehandler)
 	 ,mState(State::IS_NOT_PRESSED)
 	 ,BUTTONS(System::instance().get<hal::Buttons>())
{
}

void IdleManager::accept(std::shared_ptr<communication::Packet> packet)
{
	// Auch hier brauche ich accept eigentlich nicht oder?
}

void IdleManager::handle(hal::HAL::Event event)
{
	typedef hal::HAL::Event Event;
	typedef hal::Buttons Buttons;



	if(event == Event::BTN_START)
	{
		lib::Timer::instance().reset();

		switch(mState)
		{
		 	lib::Timer::instance().reset();  //reset und elapsed muss ich mir noch überlegen wo sie sinnvol hin müssen, vielleicht kannst du mir rückmeldung deswegen geben

		    case State::IS_NOT_PRESSED :
		      if(lib::Timer::instance().elapsed() < 5)
		      {
		    	  mState = State::IS_PRESSED;
		      }
		      lib::Timer::instance().elapsed();
	    	break;

			case State::IS_PRESSED:
				if(lib::Timer::instance().elapsed() > 5)
				{
		    	  mState = State::READY_RUN;
				}
				else
				{
					mState = State::IS_NOT_PRESSED;
				}
			break;

			case State::READY_RUN :
				if(lib::Timer::instance().elapsed() > 2000)
				{
					mState = State::READY_CONFIG;
				}
				else if(!BUTTONS.isPressed(Buttons::Button::START))
				{
					mState = State::RUN;
				}
			break;

			case State::READY_CONFIG:
				if(!BUTTONS.isPressed(Buttons::Button::START))
				{
					mState = State::CONFIG;
				}
			break;

			case State::RUN :
				mBaseHandler->accept(std::make_shared<communication::Packet>(communication::Packet::Location::BASE, communication::Packet::Location::MASTER, communication::Packet::Message::SELECT_RUN));
			break;

			case State::CONFIG :
				 mBaseHandler->accept( std::make_shared<communication::Packet>(communication::Packet::Location::BASE, communication::Packet::Location::MASTER, communication::Packet::Message::SELECT_CONFIG));
			break;

			default:
			break;

		};
	}

}

}}
