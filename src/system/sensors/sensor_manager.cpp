#include "sensor_manager.hpp"

uint8_t mt9v034_buffer_0[MT9V034_OF_MODE_DMA_BUFFER_SIZE] __attribute__((section(".ccmram")));
uint8_t mt9v034_buffer_1[MT9V034_OF_MODE_DMA_BUFFER_SIZE] __attribute__((section(".ccmram")));

SensorManager::SensorManager(I2CHost *i2c_host):
_i2c_host(i2c_host)
{
  this->_mt9v034 = new MT9V034(this->_i2c_host);
}

SensorManager::~SensorManager() {
  delete this->_mt9v034;
}

uint8_t SensorManager::init() {
  uint8_t res = 1;
  res = res && this->_mt9v034->init(mt9v034_buffer_0, mt9v034_buffer_1);
  return res;
}

MT9V034 *SensorManager::get_mt9v034() {
  return this->_mt9v034;
}