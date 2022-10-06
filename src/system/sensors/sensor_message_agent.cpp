#include "sensor_message_agent.hpp"

SensorMessageAgent::SensorMessageAgent(MessageManager *message_manager, SensorManager *sensor_manager)
:_message_manager(message_manager),
_sensor_manager(sensor_manager)
{
  this->_message_manager->register_message_agent(OPFLOW_MESSAGE_TYPE_GET_IMAGE_REQUEST, this);
}

SensorMessageAgent::~SensorMessageAgent() {}

uint8_t SensorMessageAgent::callback_func(Message *request_message, Message* response_message) {
  if (request_message->message_type == OPFLOW_MESSAGE_TYPE_GET_IMAGE_REQUEST) {
    uint32_t* frame_id = (uint32_t *)request_message->message;
    this->_sensor_manager->request_image_frame_under_hr_mode(*frame_id);
    return 0;
  }
}

void SensorMessageAgent::send_message(Message *request_message) {
  this->_message_manager->send_message(request_message);
}

uint8_t SensorMessageAgent::com_connected() {
  return 1;
}