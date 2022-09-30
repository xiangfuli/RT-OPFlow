#ifndef MESSAGE_BROKER_H
#define MESSAGE_BROKER_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "message.hpp"
#include "message_agent.hpp"

struct FuncList{
  uint32_t message_type;
  MessageAgent *agent;
  FuncList *next;
};

class MessageBroker {
public:
  MessageBroker();
  ~MessageBroker();
  uint8_t getBuckIndex(uint32_t message_type);
  uint8_t registerConsumer(uint32_t message_type, MessageAgent *agent);
  uint8_t process(Message *request_message, Message* response_message);

private:
  const int func_buck_num = (2 << 5) - 1;
  FuncList *_func_bucks;
};

#endif /* MESSAGE_BROKER_H */
