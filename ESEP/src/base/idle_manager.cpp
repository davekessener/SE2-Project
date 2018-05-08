
#include "idle_manager.h"


namespace esep{ namespace base{

		   void IdleManager::accept(communication::Packet)
		   {

		   }
		   void IdleManager::handle(hal::HAL::Event)
		   {

			   lib::Timer::instance().registerCallback([this](void){
				   this->accept(communication::Packet(communication::Packet::Location::BASE, communication::Packet::Location::BASE, communication::Packet::Message::SELECT_CONFIG))
			   },1000);

		   }

}}
