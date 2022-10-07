#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "usbd_usr.h"

#include "config_manager.hpp"
#include "message_manager.hpp"
#include "system_message_agent.hpp"
#include "sensor_manager.hpp"
#include "sensor_message_agent.hpp"
#include "optical_flow.hpp"

void system_task_wrapper (void *pvParameters);

class SystemManager {
public:
  SystemManager(OpFlowBoard *board);
  ~SystemManager();

  uint8_t init();
  void tasks_start();
  I2CHost *get_i2c_host();
  SensorManager *get_sensor_manager();
  MessageManager *get_message_manager();

  void task_lifetime_counting();
  void task_update_led_according_to_system_state();

private:
  void increase_launch_times();
  void load_launch_times();
  void load_system_lifetime();

  uint8_t _lifetime_byte_0;
  uint8_t _lifetime_byte_1;
  uint8_t _lifetime_byte_2;
  uint8_t _lifetime_byte_3;

  uint8_t _launch_time_byte_0;
  uint8_t _launch_time_byte_1;

  I2CHost *_i2c_host;
  ConfigManager *_config_manager;
  MessageManager *_message_manager;
  SystemMessageAgent *_system_message_agent;
  SensorManager *_sensor_manager;
  SensorMessageAgent *_sensor_message_agent;
  StorageManager *_storage_manager;

  OpticalFlow *_optiocal_flow;

  // system state
  SystemState _system_state;
};

#endif /* SYSTEM_MANAGER_H */
