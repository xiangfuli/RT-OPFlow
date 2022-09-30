#include "system_manager.hpp"

void system_task_lifecounting_wrapper(void *pvParameters) {
  static_cast<SystemManager *>(pvParameters)->task_lifetime_counting();
}


SystemManager::SystemManager(ConfigManager *config_manager, MessageManager *message_manager) {
  this->_config_manager = config_manager;
  this->_message_manager = message_manager;
}

SystemManager::~SystemManager() {
  
}

uint8_t SystemManager::init() {
  this->_system_message_agent = new SystemMessageAgent(this->_message_manager);

  // Load needed configurations
  this->_lifetime_byte_0 = this->_config_manager->_system_lifetime_byte_0->get_value();
  this->_lifetime_byte_1 = this->_config_manager->_system_lifetime_byte_1->get_value();
  this->_lifetime_byte_2 = this->_config_manager->_system_lifetime_byte_2->get_value();
  this->_lifetime_byte_3 = this->_config_manager->_system_lifetime_byte_3->get_value();

  this->_launch_time_byte_0 = this->_config_manager->_system_launch_times_byte_0->get_value();
  this->_launch_time_byte_1 = this->_config_manager->_system_launch_times_byte_1->get_value();

  this->increase_launch_times();

  return 1;
}

void SystemManager::increase_launch_times() {
  uint16_t times = (((uint16_t)this->_launch_time_byte_0) << 8) | ((uint16_t)this->_launch_time_byte_1);
  times++;
  this->_launch_time_byte_0 = times >> 8;
  this->_launch_time_byte_1 = times & 0x00FF;
  this->_config_manager->store_cofig(this->_config_manager->_system_launch_times_byte_0, this->_launch_time_byte_0);
  this->_config_manager->store_cofig(this->_config_manager->_system_launch_times_byte_1, this->_launch_time_byte_1);
}

void SystemManager::tasks_start() {
  xTaskCreate(
    system_task_lifecounting_wrapper,
    "system_lifetime_counting",
    configMINIMAL_STACK_SIZE,
    this, 
    configMAX_PRIORITIES, 
    NULL
  );
}

void SystemManager::task_lifetime_counting() {
  TickType_t xLastWakeTime;
	const TickType_t xFrequency = configTICK_RATE_HZ;
	while(1) {
		xLastWakeTime = xTaskGetTickCount();
		this->_lifetime_byte_3 += 1;
		this->_config_manager->store_cofig(
      this->_config_manager->_system_lifetime_byte_3,
      this->_lifetime_byte_3
    );
		if (this->_lifetime_byte_3 == 0) {
			this->_lifetime_byte_2 += 1;
			this->_config_manager->store_cofig(
        this->_config_manager->_system_lifetime_byte_2,
        this->_lifetime_byte_2
      );
			if (this->_lifetime_byte_2 == 0) {
				this->_lifetime_byte_1 += 1;
				this->_config_manager->store_cofig(
          this->_config_manager->_system_lifetime_byte_1,
          this->_lifetime_byte_1
        );
				if (this->_lifetime_byte_1 == 0) {
					this->_lifetime_byte_1 += 1;
					this->_config_manager->store_cofig(
            this->_config_manager->_system_lifetime_byte_0,
            this->_lifetime_byte_0
          );
					if (this->_lifetime_byte_0 == 0) {
						// That's impossible
					}
				}
			}
		}
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}