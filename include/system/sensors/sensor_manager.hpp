#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "stm32f4xx.h"

#include "i2c_host.hpp"
#include "mt9v034.hpp"
#include "message_manager.hpp"

#define HR_MODE_FRAME_NUMBER  16
#define OPFLOW_MESSAGE_TYPE_GET_IMAGE_REQUEST_RESPONSE                        0x00000301

class SensorManager {
public:
  SensorManager(I2CHost *i2c_host, MessageManager *message_manager);
  ~SensorManager();

  uint8_t init();
  uint8_t start_camera_capture();
  uint8_t select_camera_operation_mode(MT9V034_mode mode);
  uint8_t request_image_frame_under_hr_mode(uint32_t frame_index);

  MT9V034 *get_mt9v034();
  MT9V034_mode get_camera_opration_mode();

  uint32_t _numbers_of_received_images;
  uint32_t _hr_mode_request_frame_index;
  uint32_t _hr_mode_dma_transmission_times_index;
  MessageManager *_message_manager;

private:
  I2CHost *_i2c_host;
  MT9V034 *_mt9v034;
  MT9V034_mode _mode;
  
};

extern "C" {
  void sensor_manager_dcmi_it_handler(SensorManager *sensor_manager);
  void sensor_manager_dcmi_dma_it_handler(SensorManager *sensor_manager);
}

#endif /* SENSOR_MANAGER_H */
