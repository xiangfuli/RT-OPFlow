#include "message_manager.hpp"

MessageManager::MessageManager():
_usb_port_connected(false)
{
  this->_message_broker = new MessageBroker();
  this->_message_center = new MessageCenter();
  this->_transmission_task = new MessageTransmissionTask(
    this->_message_center,
    this->_message_broker
  );
}

MessageManager::~MessageManager() {
  delete this->_message_broker;
  delete this->_message_center;
  delete this->_transmission_task;
}

uint8_t MessageManager::init() {
  return 1;
}

bool MessageManager::all_message_has_been_sent() {
  return this->_message_center->_message_list_header->processed == 1 
    &&  this->_message_center->_message_list_header->next == nullptr;
}

uint8_t MessageManager::register_message_agent(uint32_t message_type, MessageAgent *agent) {
  return this->_message_broker->registerConsumer(message_type, agent);
}

void MessageManager::send_message(Message *message) {
  this->_message_center->send_message(message);
}

void MessageManager::update_usb_port_state(bool connected) {
  this->_message_center->update_com_connected_state(connected);

  // if port is connected, start the task. Else stop it.
  if (connected) {
    this->start_transmission_task();
  } else {
    this->stop_transmission_task();
  }
}

void MessageManager::start_transmission_task() {
  if (!this->_transmission_task_handle) {
    xTaskCreate(
      message_transmission_task_wrapper,
      "message_transmission_task",
      configMINIMAL_STACK_SIZE,
      this->_transmission_task, 
      configMAX_PRIORITIES, 
      this->_transmission_task_handle
    );
  }
}

void MessageManager::stop_transmission_task() {
  if (this->_transmission_task_handle) {
    vTaskDelete(*this->_transmission_task_handle);
  }
}

void MessageManager::receive_message_from_usb(uint8_t *buffer, uint32_t len) {
  this->_message_center->_rx_buffer->appendByteArray(buffer, len);
}

void message_transmission_task_wrapper(void *pvParameters) {
  static_cast<MessageTransmissionTask *>(pvParameters)->start_transmission();
}
