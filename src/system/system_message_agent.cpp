#include "system_message_agent.hpp"

SystemMessageAgent::SystemMessageAgent(MessageManager *message_manager) {
  this->_message_manager = message_manager;
  this->_message_manager->register_message_agent(MESSAGE_TYPE_SYSTEM_HEARTBEAT, this);
}

SystemMessageAgent::~SystemMessageAgent() {

}

uint8_t SystemMessageAgent::callbackFunc(Message *request_message, Message* response_message) {
  if (request_message->message_type == MESSAGE_TYPE_SYSTEM_HEARTBEAT) {
    response_message->message = (uint8_t *) pvPortMalloc(1);
    *response_message->message = (uint8_t) 2;
    response_message->setWholeMessageSizeWithInnerMessage(1);
  }
  return 1;
}

uint8_t SystemMessageAgent::send_message(Message *request_message) {
  this->_message_manager->send_message(request_message);
  return 1;
}

uint8_t SystemMessageAgent::comConnected() {

}