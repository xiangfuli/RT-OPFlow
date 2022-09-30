#ifndef QUEUE_HPP
#define QUEUE_HPP

#include "stm32f4xx.h"

class QueueNode {
public:
  QueueNode(void *pointer);
  ~QueueNode();

  void *_pointer;
  QueueNode *_next;
};

class Queue {
public:
  Queue();
  ~Queue();

  void push(void *pointer);
  void pop();
  void* front();
  void* last();
  uint32_t size();

private:
  uint32_t _size;
  QueueNode *_head;
  QueueNode *_tail;
};

#endif /* QUEUE_H */
