#include "config_option.hpp"

ConfigOption::ConfigOption(uint16_t address, uint8_t value, bool persistent) {
  this->_address = address;
  this->_value = value;
  this->_persistent = persistent;
}

ConfigOption::~ConfigOption() {

}

uint8_t ConfigOption::get_value() {
  return this->_value;
}

void ConfigOption::set_value(uint8_t value) {
  this->_value = value;
}

bool ConfigOption::is_persistent() {
  return this->_persistent;
}

uint16_t ConfigOption::get_address() {
  return this->_address;
}