#include "message_center.hpp"

MessageListNode::MessageListNode()
{
  this->message = nullptr;
  this->next = nullptr;
  this->processed = 0;
  this->need_to_free_message_bytes = false;
}

MessageListNode::~MessageListNode()
{
  if (this->need_to_free_message_bytes) {
    delete this->message->message;
  }
  delete this->message;
}

MessageCenter::MessageCenter():
_usb_port_connected(0)
{
  this->_rx_buffer = new CircularByteArray(1024);

  this->_message_list_header = new MessageListNode();
  this->_message_list_header->message = NULL;
  this->_message_list_header->processed = 0;
  this->_message_list_tail = this->_message_list_header;
}
void MessageCenter::message_received(uint8_t *message, uint32_t length) {
  this->_rx_buffer->appendByteArray((const uint8_t *) message, length);
}

MessageCenter::~MessageCenter() {
  delete this->_rx_buffer;
}

uint8_t MessageCenter::init() {
  return 1;
}

void MessageCenter::send_message(Message *message) {
  MessageListNode *temp = this->_message_list_tail;
  this->_message_list_tail = new MessageListNode();
  this->_message_list_tail->message = NULL;
  this->_message_list_tail->processed = 0;
  temp->message = message;
  temp->next = this->_message_list_tail;
}

void MessageCenter::update_com_connected_state(bool connected) {
  this->_usb_port_connected = connected;
}