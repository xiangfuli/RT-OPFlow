#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "stm32f4xx.h"
#include "i2c_host.hpp"
#include "mt9v034.hpp"



class SensorManager {
public:
  SensorManager(I2CHost *i2c_host);
  ~SensorManager();

  uint8_t init();
  uint8_t start_camera_capture();

  MT9V034 *get_mt9v034();

private:
  I2CHost *_i2c_host;
  MT9V034 *_mt9v034;

  // camera pixels buffer

};

#endif /* SENSOR_MANAGER_H */
