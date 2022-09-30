#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "i2c_host.hpp"
#include "queue.hpp"

using namespace std;

void storage_manager_config_storing_task_wrapper(void *pvParameters);

#define EEPROM_ADDRESS    0x54

class StorageParameter {
public:
  StorageParameter(uint16_t address, uint8_t  value);
  ~StorageParameter();

  uint16_t _address;
  uint8_t  _value;
  uint8_t _already_written;
};

class StorageManager {
public:
  StorageManager(I2CHost *i2c_host);
  ~StorageManager();

  uint8_t store_config(uint16_t address, uint8_t value);
  uint8_t load_config(uint16_t address, uint8_t *value);

  uint8_t task_start();
  void write_config_task();
  
  /**
   * @brief This method should only works for write default configs in Config Manager
   * 
   */
  uint8_t flush_configs(bool wait);
private:
  I2CHost *_i2c_host;
  Queue *_configs_to_be_stored;

  uint8_t get_next_config_to_be_wriiten(StorageParameter *&config);
};

#endif /* STORAGE_MANAGER_H */
