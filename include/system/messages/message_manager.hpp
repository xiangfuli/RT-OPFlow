#ifndef MESSAGE_MANAGER_H
#define MESSAGE_MANAGER_H

#include "stm32f4xx.h"
#include "message_center.hpp"
#include "message_broker.hpp"
#include "message_transmission_task.hpp"

void message_transmission_task_wrapper(void *pvParameters);


class MessageManager {
public:
  MessageManager();
  ~MessageManager();

  uint8_t init();
  void update_usb_port_state(bool connected);
  uint8_t register_message_agent(uint32_t message_type, MessageAgent *agent);
  void send_message(Message *message);
  void start_transmission_task();
  void receive_message_from_usb(uint8_t *buffer, uint32_t len);
private:
  
  void stop_transmission_task();

  bool _usb_port_connected;

  MessageCenter *_message_center;
  MessageBroker *_message_broker;
  MessageTransmissionTask *_transmission_task;
  TaskHandle_t *_transmission_task_handle;
};

#endif /* MESSAGE_MANAGER_H */
