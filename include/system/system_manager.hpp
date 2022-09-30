#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

#include "config_manager.hpp"
#include "message_manager.hpp"
#include "system_message_agent.hpp"

void system_task_wrapper (void *pvParameters);

class SystemManager {
public:
  SystemManager(ConfigManager *config_manager, MessageManager *message_manager);
  ~SystemManager();

  uint8_t init();
  void tasks_start();

  void task_lifetime_counting();

private:
  void increase_launch_times();

  uint8_t _lifetime_byte_0;
  uint8_t _lifetime_byte_1;
  uint8_t _lifetime_byte_2;
  uint8_t _lifetime_byte_3;

  uint8_t _launch_time_byte_0;
  uint8_t _launch_time_byte_1;
  
  ConfigManager *_config_manager;
  MessageManager *_message_manager;
  SystemMessageAgent *_system_message_agent;

};

#endif /* SYSTEM_MANAGER_H */
