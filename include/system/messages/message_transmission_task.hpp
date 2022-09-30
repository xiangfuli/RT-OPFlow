#ifndef MESSAGE_TRANSMISSION_TASK_H
#define MESSAGE_TRANSMISSION_TASK_H

#include "usbd_cdc_vcp.h"

#include "message_center.hpp"
#include "message_broker.hpp"


#define MESSAGECENTER_TRANSMITTER_RESPONSE_MSG_STARTED 0x01
#define MESSAGECENTER_TRANSMITTER_RESPONSE_MSG_INNER_MSG_STARTED 0x02
#define MESSAGECENTER_TRANSMITTER_RESPONSE_MSG_END_MAGIC_NUMBER_STARTED 0x03
#define MESSAGECENTER_TRANSMITTER_RESPONSE_MSG_IDLE 0x04

#ifndef USB_CDC_IDLE
#define USB_CDC_IDLE         0
#endif

extern uint8_t USB_Tx_State;

#define USBD_SINGLE_PAC_MAX_SIZE  64

class MessageTransmissionTask {
public:
  MessageTransmissionTask(MessageCenter *message_center, MessageBroker *message_broker);
  ~MessageTransmissionTask();

  void start_transmission();
private:
  /**
   * @brief Get the next bytes to be sent object
   * 
   * @param message 
   * @param already_sent_bytes_nnumber 
   * @param bytes_to_be_sent_buffer 
   * @param len 
   * @return uint8_t return 1 when there are left bytes not sent, else 0
   */
  uint8_t get_next_bytes_to_be_sent(Message *message, uint32_t already_sent_bytes_number, uint8_t *bytes_to_be_sent_buffer, uint8_t *len);

  uint8_t receiveMsgFromBuffer(CircularByteArray *buffer, Message *request_message);

  MessageCenter *_message_center;
  MessageBroker *_message_broker;

  uint8_t transmittion_state = MESSAGECENTER_TRANSMITTER_RESPONSE_MSG_IDLE;

  uint32_t START_MAGIC_NUMBER = MESSAGE_START_MAGIC_NUMBER;
  uint32_t END_MAGIC_NUMBER = MESSAGE_ENDING_MAGIC_NUMBER;

  // because of little-endian
  uint8_t first_byte_of_message_magic_start_number = (uint8_t) (MESSAGE_START_MAGIC_NUMBER);
  uint8_t second_byte_of_message_magic_start_number = (uint8_t) (MESSAGE_START_MAGIC_NUMBER >> 8);
  uint8_t third_byte_of_message_magic_start_number = (uint8_t) (MESSAGE_START_MAGIC_NUMBER >> 16);
  uint8_t fourth_byte_of_message_magic_start_number = (uint8_t) (MESSAGE_START_MAGIC_NUMBER >> 24);
};

#endif /* MESSAGE_TRANSMISSION_TASK_H */
