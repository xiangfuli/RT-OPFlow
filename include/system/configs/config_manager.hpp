#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "storage_manager.hpp"
#include "config_option.hpp"
#include <list>

using namespace std;

class ConfigManager {
public:
  ConfigManager(StorageManager *storage_manager);
  ~ConfigManager();

  // Configs
  ConfigOption *_system_firmware_version_byte_0;
  ConfigOption *_system_firmware_version_byte_1;
  
  ConfigOption *_system_lifetime_byte_0;
  ConfigOption *_system_lifetime_byte_1;
  ConfigOption *_system_lifetime_byte_2;
  ConfigOption *_system_lifetime_byte_3;

  ConfigOption *_system_launch_times_byte_0;
  ConfigOption *_system_launch_times_byte_1;

  uint8_t init();
  uint8_t store_cofig(ConfigOption *config, uint8_t new_value);
  

private:
  uint8_t write_default_configs();
  void create_config(uint16_t address, uint8_t value, bool persistent, ConfigOption *&config);
  uint8_t load_cofigs();

  uint16_t _system_firmware_version = 0x0001;
  list<ConfigOption *> _configs;
  StorageManager *_storage_manager;
  
  
};

#endif /* CONFIG_MANAGER_H */
