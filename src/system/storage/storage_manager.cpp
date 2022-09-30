#include "storage_manager.hpp"

StorageParameter::StorageParameter(uint16_t address, uint8_t value):
 _address(address), 
 _value(value),
 _already_written(0) {
  
}

StorageParameter::~StorageParameter() {

}

void storage_manager_config_storing_task_wrapper(void *pvParameters) {
  static_cast<StorageManager *>(pvParameters)->write_config_task();
}

StorageManager::StorageManager(I2CHost *i2c_host) {
  this->_i2c_host = i2c_host;
  this->_configs_to_be_stored = new Queue();
}

StorageManager::~StorageManager() {
  delete this->_configs_to_be_stored;
}

uint8_t StorageManager::store_config(uint16_t address, uint8_t value) {
  StorageParameter *temp = new StorageParameter(address, value);
  this->_configs_to_be_stored->push(temp);
  return 1;
}

uint8_t StorageManager::load_config(uint16_t address, uint8_t *value) {
  return this->_i2c_host->eeprom_read(
    EEPROM_ADDRESS,
    address,
    value,
    1
  );
}

uint8_t StorageManager::task_start() {
  xTaskCreate(
    storage_manager_config_storing_task_wrapper,
    "storage_manager_config_storing_task",
    configMINIMAL_STACK_SIZE,
    this, 
    configMAX_PRIORITIES, 
    NULL
  );
  return 1;
}

uint8_t StorageManager::get_next_config_to_be_wriiten(StorageParameter *&config) {
  if (this->_configs_to_be_stored->size() != 0) {
      config = (StorageParameter *)this->_configs_to_be_stored->front();
      if (!config->_already_written) {
        return 1;
      } else {
        if (this->_configs_to_be_stored->size() > 1) {
          StorageParameter *temp = (StorageParameter *)this->_configs_to_be_stored->front();
          this->_configs_to_be_stored->pop();
          delete temp;
          config = (StorageParameter *)this->_configs_to_be_stored->front();
          return 1;
        } else {
          config = nullptr;
          return 0;
        }
      }
  } else {
    return 0;
  }
}

uint8_t StorageManager::flush_configs(bool wait) {
  StorageParameter *config;
  uint8_t res = 1;
  while(get_next_config_to_be_wriiten(config) && res) {
    res = res & this->_i2c_host->eeprom_write(
            EEPROM_ADDRESS,
            config->_address,
            config->_value
          );
    us_sleep(10 * 1000); // sleep 10 ms
    config->_already_written = 1;
  }
  return res;
}

void StorageManager::write_config_task() {
  StorageParameter *config;
  while(1) {
    // Thread safe
    if (get_next_config_to_be_wriiten(config)) {
      this->_i2c_host->eeprom_write(
        EEPROM_ADDRESS,
        config->_address,
        config->_value
      );
      config->_already_written = 1;
    }

    vTaskDelay(pdMS_TO_TICKS(5));
  }
}