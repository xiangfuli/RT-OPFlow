#ifndef SYSTEM_MESSAGE_AGENT_H
#define SYSTEM_MESSAGE_AGENT_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"

#include "message_agent.hpp"
#include "message_manager.hpp"

#define MESSAGE_TYPE_SYSTEM_HEARTBEAT           0x00000100

class SystemMessageAgent: public MessageAgent {
public:
  SystemMessageAgent(MessageManager *message_manager);
  ~SystemMessageAgent();

  uint8_t callback_func(Message *request_message, Message* response_message);
  void send_message(Message *request_message);
  uint8_t com_connected();

private:
  MessageManager *_message_manager;
};

#endif /* SYSTEM_MESSAGE_AGENT_H */
