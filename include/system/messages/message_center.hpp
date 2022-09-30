#ifndef MESSAGE_CENTER_H
#define MESSAGE_CENTER_H

#include "message_broker.hpp"
#include "circular_array.hpp"

class MessageListNode {
public:
  Message *message;
  uint8_t processed;
  MessageListNode *next;
public:
  MessageListNode();
  ~MessageListNode();
};

/**
 * @brief This class is used to store the received message and the messages to be sent
 * 
 */
class MessageCenter {
public:
  MessageCenter();
  ~MessageCenter();

  uint8_t init();
  void send_message(Message *message);
  void message_received(uint8_t *message, uint32_t length);
  void update_com_connected_state(bool connected);

  MessageListNode *_message_list_header;
  MessageListNode *_message_list_tail;
  CircularByteArray *_rx_buffer;
private:
  TaskHandle_t _transmission_task;

  uint8_t _usb_port_connected;
};

#endif /* MESSAGE_CENTER_H */
