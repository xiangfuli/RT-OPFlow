#ifndef CONFIG_OPTION_H
#define CONFIG_OPTION_H

#include "stm32f4xx.h"

class ConfigOption {
public:
  ConfigOption(uint16_t address, uint8_t value, bool persistent);
  ~ConfigOption();

  uint8_t get_value();
  void set_value(uint8_t value);
  bool is_persistent();
  uint16_t get_address();
private:
  uint16_t _address;
  uint8_t _value;
  bool _persistent;
};

#endif /* CONFIG_OPTION_H */
