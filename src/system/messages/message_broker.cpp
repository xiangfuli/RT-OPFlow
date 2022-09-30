#include "message_broker.hpp"

#define CONSUMER_FUNCTION_BUCK_SIZE  (1 << 5) - 1

MessageBroker::MessageBroker()
{
  this->_func_bucks = new FuncList[CONSUMER_FUNCTION_BUCK_SIZE];
  for (int i=0; i<CONSUMER_FUNCTION_BUCK_SIZE; i++) {
    this->_func_bucks[i].agent = nullptr;
  }
}

uint8_t MessageBroker::getBuckIndex(uint32_t message_type)
{
  return (uint8_t) (message_type & CONSUMER_FUNCTION_BUCK_SIZE);
}

uint8_t MessageBroker::registerConsumer(uint32_t message_type, MessageAgent *agent) {
  uint8_t buck_index = this->getBuckIndex(message_type);
  FuncList *func_node = &this->_func_bucks[buck_index];
  
  // find the tail node
  while(func_node->agent) {
      func_node = func_node->next;
  }

  // register the consumer callback function
  FuncList *func_list_node = (FuncList *) pvPortMalloc(sizeof(FuncList));
  func_node->message_type = message_type;
  func_node->agent = agent;
  func_node->next = nullptr;

  if (!func_node->next) {
    func_node->next = (FuncList *) pvPortMalloc(sizeof(FuncList));
    func_node->next->agent = nullptr;
  }

  return 1;
}

uint8_t MessageBroker::process(Message *request_message, Message* response_message) {
  // first find the callback function
  uint8_t buck_index = this->getBuckIndex(request_message->message_type);
  FuncList *func_node = &this->_func_bucks[buck_index];
  uint8_t response_result = 0;
  // find the mapped callback function
  while(func_node->agent) {
      if (func_node->message_type == request_message->message_type) {
        response_result = func_node->agent->callbackFunc(request_message, response_message);
        break;
      }
      func_node = func_node->next;
  }
  // no mapped callback function
  if (func_node->agent == nullptr) {
    response_result = 255;
  }
  return response_result;
}
