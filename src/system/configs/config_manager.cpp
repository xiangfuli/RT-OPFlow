#include "config_manager.hpp"

ConfigManager::ConfigManager(StorageManager *storage_manager):
_storage_manager(storage_manager) {

}

ConfigManager::~ConfigManager() {

}

uint8_t ConfigManager::init() {
  // create all the configs
  this->create_config(0x0000, 0, true, this->_system_firmware_version_byte_0);
  this->create_config(0x0001, 1, true, this->_system_firmware_version_byte_1);

  this->create_config(0x0010, 0, true, this->_system_lifetime_byte_0);
  this->create_config(0x0011, 0, true, this->_system_lifetime_byte_1);
  this->create_config(0x0012, 0, true, this->_system_lifetime_byte_2);
  this->create_config(0x0013, 0, true, this->_system_lifetime_byte_3);

  this->create_config(0x0014, 0, true, this->_system_launch_times_byte_0);
  this->create_config(0x0015, 0, true, this->_system_launch_times_byte_1);

  // first get firmware version
  uint8_t res = 0;
  uint8_t fm_v_0, fm_v_1;
  res = this->_storage_manager->load_config(0x0000, &fm_v_1);
  res = res && this->_storage_manager->load_config(0x0001, &fm_v_0);
  
  if (this->_system_firmware_version != ((((uint16_t) fm_v_1) << 8) | fm_v_0)) {
    write_default_configs();
    this->_storage_manager->flush_configs(1);
  }

  res = res & this->load_cofigs();

  return res;
}

void ConfigManager::create_config(uint16_t address, uint8_t value, bool persistent, ConfigOption *&config) {
  config = new ConfigOption(address, value, persistent);
  this->_configs.push_back(config);
}

uint8_t ConfigManager::store_cofig(ConfigOption *config, uint8_t new_value) {
  uint8_t res = 1;
  config->set_value(new_value);
  if (config->is_persistent()) {
    this->_storage_manager->store_config(config->get_address(), new_value);
  }
  return 1;
}

uint8_t ConfigManager::write_default_configs() {
  // write system firmware version first
  for(ConfigOption *config : this->_configs) {
    this->_storage_manager->store_config(config->get_address(), config->get_value());
  }
  return 1;
}

uint8_t ConfigManager::load_cofigs() {
  uint8_t res = 1;;
  for(ConfigOption *config : this->_configs) {
    uint8_t value;
    res = res && this->_storage_manager->load_config(config->get_address(), &value);
    config->set_value(value);
  }
  return res;
}



