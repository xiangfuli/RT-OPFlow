#ifndef SENSOR_MESSAGE_AGENT_H
#define SENSOR_MESSAGE_AGENT_H

#include "stm32f4xx.h"
#include "message_agent.hpp"
#include "message_manager.hpp"
#include "sensor_manager.hpp"

#define OPFLOW_MESSAGE_TYPE_GET_IMAGE_REQUEST                                 0x00000300

class SensorMessageAgent: public MessageAgent{
public:
  SensorMessageAgent(MessageManager *message_manager, SensorManager *sensor_manager);
  ~SensorMessageAgent();

  uint8_t callback_func(Message *request_message, Message* response_message);
  void send_message(Message *request_message);
  uint8_t com_connected();

private:
  MessageManager *_message_manager;
  SensorManager *_sensor_manager;
};

#endif /* SENSOR_MESSAGE_AGENT_H */
