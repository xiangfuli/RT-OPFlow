#include "sensor_manager.hpp"

uint8_t mt9v034_buffer_0[MT9V034_OF_MODE_DMA_BUFFER_SIZE];
uint8_t mt9v034_buffer_1[MT9V034_OF_MODE_DMA_BUFFER_SIZE];

SensorManager::SensorManager(I2CHost *i2c_host, MessageManager *message_manager):
_i2c_host(i2c_host),
_message_manager(message_manager),
_numbers_of_received_images(0),
_hr_mode_dma_transmission_times_index(0),
_hr_mode_request_frame_index(0),
_mode(FLOW_MODE)
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

uint8_t SensorManager::select_camera_operation_mode(SystemOperationMode mode) {
  if (this->_mode != mode) {
    this->_numbers_of_received_images = 0;
    this->_hr_mode_dma_transmission_times_index = 0;
    this->_mt9v034->select_mode(mode);
    this->_mode = mode;
  }
  return 1;
}

MT9V034 *SensorManager::get_mt9v034() {
  return this->_mt9v034;
}

SystemOperationMode SensorManager::get_camera_opration_mode() {
  return this->_mode;
}

uint8_t SensorManager::start_camera_capture() {
  this->_mt9v034->start_image_capture();
  return 1;
}

void SensorManager::request_latest_image_buffer_under_flow_mode(uint8_t *buffer) {
  if (this->_mode == FLOW_MODE) {
    this->_request_latest_image_frame_buffer = buffer;
    this->_request_latest_image_frame = true;
    this->_latest_frame_arrived = false;

    while (!this->_latest_frame_arrived) {
      this->_latest_frame_arrived = false;
    }

    this->_request_latest_image_frame = false;
    this->_latest_frame_arrived = true;
  }
}

uint8_t SensorManager::request_image_frame_under_hr_mode(uint32_t frame_index) {
  this->_hr_mode_request_frame_index = frame_index;
  this->_hr_mode_dma_transmission_times_index = 0;
  this->start_camera_capture();
  return 1;
}

void sensor_manager_dcmi_it_handler(SensorManager *sensor_manager) {
  /**
   * By default, only frame interrupt is enabled.
   */
  if (DCMI_GetITStatus(DCMI_IT_FRAME) != RESET) {
		DCMI_ClearITPendingBit(DCMI_IT_FRAME);
    sensor_manager->_numbers_of_received_images++;
    /**
     * Clear the frame index used in the HR mode.
     */
    if (sensor_manager->get_camera_opration_mode() == HR_MODE 
        && sensor_manager->_hr_mode_dma_transmission_times_index < sensor_manager->_hr_mode_request_frame_index) {
      sensor_manager->start_camera_capture();
    } else if (sensor_manager->get_camera_opration_mode() == FLOW_MODE) {
      
    }
	}
}

void sensor_manager_dcmi_dma_it_handler(SensorManager *sensor_manager) {
  /**
   *  handle the transfer completion flag 
   *  While working under HR mode, device will take 16 picture and transfer 1/16 each time to the remote endpoint. 
   **/
  if (DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1) == SET) {
    if (sensor_manager->get_camera_opration_mode() == HR_MODE) {
      sensor_manager->_hr_mode_dma_transmission_times_index++;
      if (sensor_manager->_hr_mode_request_frame_index 
          == sensor_manager->_hr_mode_dma_transmission_times_index) {
        memcpy(mt9v034_buffer_1, mt9v034_buffer_0, MT9V034_HR_MODE_DMA_BUFFER_SIZE);
        Message *message = new Message();
        message->defaultInit();
        message->message_type = OPFLOW_MESSAGE_TYPE_GET_IMAGE_REQUEST_RESPONSE;
        message->message_frame_index = sensor_manager->_hr_mode_request_frame_index;
        message->message_total_frame = HR_MODE_FRAME_NUMBER;
        message->message = mt9v034_buffer_1;
        message->setWholeMessageSizeWithInnerMessage(MT9V034_HR_MODE_DMA_BUFFER_SIZE);
        sensor_manager->_message_manager->send_message(message);
      }
    } else if (sensor_manager->get_camera_opration_mode() == FLOW_MODE) {
      if (sensor_manager->_request_latest_image_frame) {
        memcpy(sensor_manager->_request_latest_image_frame_buffer, mt9v034_buffer_0, MT9V034_OF_MODE_DMA_BUFFER_SIZE);
        sensor_manager->_latest_frame_arrived = true;
      }
    }

    DMA_ClearITPendingBit(DMA2_Stream1, DMA_IT_TCIF1);
  }
}