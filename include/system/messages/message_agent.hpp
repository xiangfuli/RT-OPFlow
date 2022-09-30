#ifndef MESSAGE_AGENT_H
#define MESSAGE_AGENT_H

#include "stm32f4xx.h"
#include "message.hpp"

class MessageAgent {
public:
  /**
   * @brief invoke callback function to deal with messages
   * 
   * @param request_message received message
   * @param response_message message to be sent
   * @return uint8_t If need to send response message, plz return 0, otherwise plz return 1
   */
  virtual uint8_t callbackFunc(Message *request_message, Message* response_message) = 0;
  virtual uint8_t send_message(Message *request_message) = 0;
  virtual uint8_t comConnected() = 0;
};

#endif /* MESSAGE_AGENT_H */