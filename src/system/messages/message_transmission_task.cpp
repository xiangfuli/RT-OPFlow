#include "message_transmission_task.hpp"

MessageTransmissionTask::MessageTransmissionTask(MessageCenter *message_center, MessageBroker *message_broker)
{
  this->_message_broker = message_broker;
  this->_message_center = message_center;
}

MessageTransmissionTask::~MessageTransmissionTask() {

}

uint8_t MessageTransmissionTask::get_next_bytes_to_be_sent(Message *message, uint32_t already_sent_bytes_number, uint8_t *bytes_to_be_sent_buffer, uint8_t *len) {
  uint8_t ending_magic_number_size = sizeof(message->ending_magic_number);
  if (message->message_size < USBD_SINGLE_PAC_MAX_SIZE) {
    memcpy(bytes_to_be_sent_buffer, &message->start_magic_number, message->getNumberOfBytesBeforeInnerMessage());
    memcpy(bytes_to_be_sent_buffer + message->getNumberOfBytesBeforeInnerMessage(),
      message->message,
      message->getInnerMessageSizeBySubtraction());
    memcpy(bytes_to_be_sent_buffer + message->message_size - ending_magic_number_size,  
      &message->ending_magic_number, 
      ending_magic_number_size);
    *len = message->message_size;
    return 1;
  } else {
    if (already_sent_bytes_number < message->message_size) {
      if (already_sent_bytes_number + 64 >= message->message_size) {
        // last several bytes need to be sent
        *len = message->message_size - already_sent_bytes_number;
        memcpy(bytes_to_be_sent_buffer, 
          message->message + already_sent_bytes_number - message->getNumberOfBytesBeforeInnerMessage(), 
          *len - ending_magic_number_size);
        memcpy(bytes_to_be_sent_buffer + *len - ending_magic_number_size,
          &message->ending_magic_number,
          ending_magic_number_size);
        return 1;
      } else {
        // have to split the message into packages
        if (already_sent_bytes_number == 0) {
          memcpy(bytes_to_be_sent_buffer, &message->start_magic_number, message->getNumberOfBytesBeforeInnerMessage());
          memcpy(bytes_to_be_sent_buffer + message->getNumberOfBytesBeforeInnerMessage(),
            message->message, 
            USBD_SINGLE_PAC_MAX_SIZE - message->getNumberOfBytesBeforeInnerMessage());
          *len = USBD_SINGLE_PAC_MAX_SIZE;
          return 1;
        } else {
          // inner message size is too large to be sent once in 64 bytes packet
          memcpy(bytes_to_be_sent_buffer, message->message + already_sent_bytes_number, 64);
          *len = USBD_SINGLE_PAC_MAX_SIZE;
          return 1;
        }
      }
    }
  }
  return 0;
}

void MessageTransmissionTask::start_transmission() {
  Message *request_message = new Message();
  request_message->defaultInit();
  uint32_t bytes_already_sent = 0;
  uint8_t bytes_buffer[64];
  uint8_t bytes_number = 0;

  while(1) {
    if (this->_message_center->_message_list_header->processed != 1 
        && this->_message_center->_message_list_header->message) {
      // analyse the pointer location in USBD module
      if (USB_Tx_State == USB_CDC_IDLE) {     // all data in tx buffer has been sent out
        if (get_next_bytes_to_be_sent(this->_message_center->_message_list_header->message, bytes_already_sent, bytes_buffer, &bytes_number)) {
          bytes_already_sent += bytes_number;
          for (int i=0; i<bytes_number; i++) {
            VCP_put_char(bytes_buffer[i]);
          }
        }
      }
    } else if (this->_message_center->_message_list_header->processed == 1 
                && this->_message_center->_message_list_header->next) {
      bytes_already_sent = 0;
      MessageListNode *temp = this->_message_center->_message_list_header;
      this->_message_center->_message_list_header = this->_message_center->_message_list_header->next;
      delete temp;
    }
    if (bytes_already_sent == this->_message_center->_message_list_header->message->message_size) {
      this->_message_center->_message_list_header->processed = 1;
    }

    // data reception
    uint8_t read_res = 0;
    read_res = receiveMsgFromBuffer(this->_message_center->_rx_buffer, request_message);

    if (read_res == 1) {
      Message *response_message = (Message *)pvPortMalloc(sizeof(Message));
      response_message->defaultInit();
      if (this->_message_broker->process(request_message, response_message) == 1) {
        this->_message_center->send_message(response_message);
      }
    }
    if (request_message->message != nullptr) {
      vPortFree(request_message->message);
      request_message->message = nullptr;
    }
    vTaskDelay(configTICK_RATE_HZ / 2000);
  }
}

uint8_t MessageTransmissionTask::receiveMsgFromBuffer(CircularByteArray *buffer, Message *request_message) {
  // start magic number and the msg length bytes
  const uint8_t number_of_bytes_of_msg_initials = 6;
  uint8_t reception_buffer[12];

  // find the start magic number
  uint8_t bytes_read_result = buffer->peek(12, reception_buffer, 0);
  if (bytes_read_result == 0) {
    return bytes_read_result;
  } else {
    int i=0;
    for (; i<number_of_bytes_of_msg_initials; i++) {
      if (!(reception_buffer[i] ^ first_byte_of_message_magic_start_number)) {
        // check if it's the start magic number
        break;
      }
    }
    if (i > 0) {
      // remove the useless bytes
      buffer->remove(i);
      return 0;
    }
  }

  // try to complete getting the first number_of_bytes_of_msg_initials bytes1A
  // find the
  if (memcmp(&START_MAGIC_NUMBER, reception_buffer, sizeof(request_message->start_magic_number)) != 0) {
    buffer->remove(1);
    return 0;
  }

  // we have found the start magic number!!!
  // get the message size
  uint16_t message_size = 0;
  memcpy(&message_size, (reception_buffer + sizeof(request_message->start_magic_number)), sizeof(request_message->message_size));
  // have to be initialized, seems like the compiler has set static memory address to end_magic_number_buffer
  // so it will affect the check result when last check is OK beucase of the remaining bytes
  const uint8_t end_magic_number_size = sizeof(END_MAGIC_NUMBER);
  uint8_t end_magic_number_buffer[end_magic_number_size];
  for (int k=0; k < end_magic_number_size; k++) {
    end_magic_number_buffer[k] = 0;
  }
  uint8_t end_magic_number_read_result = buffer->peek(end_magic_number_size, end_magic_number_buffer, message_size - sizeof(uint32_t));
  if (end_magic_number_read_result == 0) {
    // it's not a complete message
    return 0;
  }
  if (memcmp(&END_MAGIC_NUMBER, end_magic_number_buffer, end_magic_number_size) != 0) {
    // it's not the right start magic number
    buffer->remove(end_magic_number_size);
    return 0;
  }

  // now it's the real message
  request_message->start_magic_number = START_MAGIC_NUMBER;
  request_message->ending_magic_number = END_MAGIC_NUMBER;
  request_message->message_size = message_size;
  buffer->peek(2, (uint8_t *)&(request_message->message_version), number_of_bytes_of_msg_initials);
  buffer->peek(4, (uint8_t *)&(request_message->message_id), number_of_bytes_of_msg_initials + 2);
  buffer->peek(2, (uint8_t *)&(request_message->message_frame_index), number_of_bytes_of_msg_initials + 2 + sizeof(uint32_t));
  buffer->peek(2, (uint8_t *)&(request_message->message_total_frame), number_of_bytes_of_msg_initials + 2 + sizeof(uint32_t) + 2);
  buffer->peek(4, (uint8_t *)&(request_message->message_type), number_of_bytes_of_msg_initials + 2 + sizeof(uint32_t) * 2);
  uint32_t message_byte_length = request_message->getInnerMessageSizeBySubtraction();
  request_message->message = (uint8_t *)pvPortMalloc(message_byte_length);
  buffer->peek(message_byte_length, request_message->message, number_of_bytes_of_msg_initials + 2 + sizeof(uint32_t) * 3);
  buffer->remove(message_size);

  return 1;
}