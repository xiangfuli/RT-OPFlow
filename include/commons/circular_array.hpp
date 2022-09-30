#ifndef CIRCULAR_ARRAY_H
#define CIRCULAR_ARRAY_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "string.h"
#include "queue.h"


class CircularByteArray {
public: 
  CircularByteArray(uint32_t size);
  ~CircularByteArray();
  
  void freeCircularByteArray();

  uint8_t appendByteArray(const uint8_t* data, uint32_t size);
  uint8_t peek(uint16_t size, uint8_t *result, uint16_t offset);
  uint8_t remove(uint16_t size);
  uint32_t leftSpace();
  uint32_t numberOfBytesContained();

private:
  int32_t _header_index;
  int32_t _tail_index;
  uint8_t *_byte_array;
  uint32_t _array_size;
  uint32_t _length;
  SemaphoreHandle_t _mutex;
};

#endif /* CIRCULAR_ARRAY_H */
