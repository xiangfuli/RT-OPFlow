#ifndef MESSAGE_H
#define MESSAGE_H

#define MESSAGE_START_MAGIC_NUMBER 0xFFFEFDFC
#define MESSAGE_ENDING_MAGIC_NUMBER 0x1A121314

#include "stm32f4xx.h"

class Message {
public:
  Message();
  ~Message();

  uint32_t start_magic_number;
  uint16_t message_size;
  uint16_t message_version;
  uint32_t message_id;
  uint16_t message_frame_index;
  uint16_t message_total_frame;
  uint32_t message_type;                                      // first 31 bits indicate the message type and the last 1 bit implies if this message is request or response message
  uint8_t *message;
  uint32_t ending_magic_number;

  void defaultInit();
  void setWholeMessageSizeWithInnerMessage(uint32_t inner_msg_size);
  uint8_t getNumberOfBytesBeforeInnerMessage();
  uint32_t getInnerMessageSizeBySubtraction();
};

#endif /* MESSAGE_H */
