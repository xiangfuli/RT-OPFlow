  #include "queue.hpp"
  
QueueNode::QueueNode(void *pointer):
_pointer(pointer),
_next(nullptr) {

}

QueueNode::~QueueNode() {

}


Queue::Queue():
_size(0),
_head(nullptr),
_tail(nullptr)
{
  this->_size = 0;
}

Queue::~Queue() {}


void Queue::push(void *pointer) {
  if (this->_head == nullptr) {
    this->_head = new QueueNode(pointer);
    this->_tail = this->_head;
  } else {
    QueueNode *temp = this->_tail;
    this->_tail = new QueueNode(pointer);
    temp->_next = this->_tail;
  }
  this->_size++;
}

void Queue::pop() {
  if (this->_size == 1) {
    delete this->_head;
    this->_head = nullptr;
    this->_tail = nullptr;
  }
  else if (this->_head != nullptr) {
    QueueNode *temp = this->_head;
    this->_head = temp->_next;
    delete temp;
  }
  this->_size--;
}

void* Queue::front() {
  if (this->_size > 0 && this->_head != nullptr) { 
    return this->_head->_pointer;
  } else {
    return nullptr;
  }
}

void* Queue::last() {
  if (this->_size > 0 && this->_tail != nullptr) { 
    return this->_tail->_pointer;
  } else {
    return nullptr;
  }
}

uint32_t Queue::size() {
  return this->_size;
}