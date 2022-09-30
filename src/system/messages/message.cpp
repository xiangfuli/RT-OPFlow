#include "message.hpp"

Message::Message()
{

}

Message::~Message()
{

}

void Message::defaultInit()
{
  this->start_magic_number = MESSAGE_START_MAGIC_NUMBER;
  this->ending_magic_number = MESSAGE_ENDING_MAGIC_NUMBER;
  this->message_id = 0;
  this->message_version=1;
  this->message_type = 0;
  this->message_total_frame = 1;
  this->message_frame_index = 1;
  this->message = nullptr;
  setWholeMessageSizeWithInnerMessage(0);
}

void Message::setWholeMessageSizeWithInnerMessage(uint32_t inner_msg_size)
{
  this->message_size = inner_msg_size
      + sizeof(this->start_magic_number)
      + sizeof(this->message_id)
      + sizeof(this->message_size)
      + sizeof(this->message_type)
      + sizeof(this->message_version)
      + sizeof(this->message_frame_index)
      + sizeof(this->message_total_frame)
      + sizeof(this->ending_magic_number);
}

uint8_t Message::getNumberOfBytesBeforeInnerMessage()
{
  return sizeof(this->start_magic_number)
          + sizeof(this->message_id)
          + sizeof(this->message_size)
          + sizeof(this->message_type)
          + sizeof(this->message_frame_index)
          + sizeof(this->message_total_frame)
          + sizeof(this->message_version);
}

uint8_t Message::getInnerMessageSizeBySubtraction()
{
  return this->message_size
          - sizeof(this->start_magic_number)
          - sizeof(this->ending_magic_number)
          - sizeof(this->message_id)
          - sizeof(this->message_size)
          - sizeof(this->message_type)
          - sizeof(this->message_frame_index)
          - sizeof(this->message_total_frame)
          - sizeof(this->message_version);
}
