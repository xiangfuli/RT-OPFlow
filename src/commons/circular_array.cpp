#include "circular_array.hpp"

CircularByteArray::CircularByteArray(uint32_t size) {
    this->_byte_array = (uint8_t *)pvPortMalloc(sizeof(uint8_t) * size);
    this->_array_size = size;
    this->_header_index = -1;
    this->_tail_index = 0;
    this->_length = 0;
    this->_mutex = xSemaphoreCreateMutex();
}

CircularByteArray::~CircularByteArray() {
  this->freeCircularByteArray();
}

void CircularByteArray::freeCircularByteArray() {
    vPortFree(_byte_array);
}

uint8_t CircularByteArray::appendByteArray(const uint8_t* data, uint32_t size) {
  xSemaphoreTake(this->_mutex, ( TickType_t ) 0);

  if (size > this->leftSpace()) {
    xSemaphoreGive(this->_mutex);
    return 0;
  }

  int32_t length_to_the_end = this->_array_size - 1 - this->_header_index;
  if (length_to_the_end > size) {
    memcpy(this->_byte_array + this->_header_index + 1, data, size);
    this->_header_index += size;
  } else if (length_to_the_end == size) {
    memcpy(this->_byte_array + this->_header_index + 1, data, size);
    this->_header_index = -1;
  } else {
    memcpy(this->_byte_array + this->_header_index + 1, data, length_to_the_end);
    uint32_t size_from_start = size - length_to_the_end;
    memcpy(this->_byte_array, data + length_to_the_end, size_from_start);
    this->_header_index = size_from_start - 1;
  }
  this->_length += size;
  xSemaphoreGive(this->_mutex);
  return 1;
}

uint8_t CircularByteArray::peek(uint16_t size, uint8_t *result, uint16_t offset) {
  if (this->numberOfBytesContained() < (size + offset)) {
    return 0;
  }

  if (this->_header_index > this->_tail_index) {
    memcpy(result, this->_byte_array + this->_tail_index + offset, size);
  } else {
    int32_t length_to_the_end = this->_array_size - this->_tail_index - offset;
    if (length_to_the_end >= size) {
      memcpy(result, this->_byte_array + this->_tail_index + offset, size);
    } else if (length_to_the_end <= 0) {
      memcpy(result, this->_byte_array - length_to_the_end , size);
    } else {
      int32_t number_of_second_copied_bytes = size - length_to_the_end;
      memcpy(result, this->_byte_array + this->_tail_index + offset, length_to_the_end);
      memcpy(result + length_to_the_end, this->_byte_array, number_of_second_copied_bytes);
    }
  }


  return 1;
}

uint8_t CircularByteArray::remove(uint16_t size) {
  if (size == 0) {
    return 1;
  }
  xSemaphoreTake(this->_mutex, ( TickType_t )0);

  if (this->numberOfBytesContained() < size) {
    xSemaphoreGive(this->_mutex);
    return 0;
  }

  if (this->_header_index > this->_tail_index) {
    this->_tail_index += size;
  } else {
    uint16_t length_to_the_end = this->_array_size - this->_tail_index;
    if (length_to_the_end > size) {
      this->_tail_index += size;
    } else {
      this->_tail_index = 0 + size - length_to_the_end;
    }
  }

  this->_length -= size;
  xSemaphoreGive(this->_mutex);
  return 1;
}

uint32_t CircularByteArray::leftSpace() {
  return this->_array_size - this->_length;
}

uint32_t CircularByteArray::numberOfBytesContained() {
  return this->_length;
}