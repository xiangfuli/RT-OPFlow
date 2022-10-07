#ifndef MAIN_H
#define MAIN_H
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "FreeRTOS.h"
#include "task.h"

#include "i2c_host.hpp"

#include "OpFlowBoardAlpha.hpp"
#include "system_manager.hpp"

extern "C" {
  void I2C2_ER_IRQHandler();
  void DMA2_Stream1_IRQHandler(void);
  void DCMI_IRQHandler(void);
  void message_manager_receive_usb_data_wrapper(uint8_t *buffer, uint32_t len);
}

/**
 * @brief Global cpp operator new redefinition
 * 
 * @param size size of class
 * @return void* generated obhect reference pointer
 */
void *operator new(size_t size);

/**
 * @brief Global cpp operator delete redefinition
 * 
 * @param ptr pointer to the object to be freed
 */
void operator delete(void *ptr);

/**
 * @brief Main entry function
 * 
 * @param ac number of input arguments
 * @param av arguments
 * @return int execution result
 */
int main(void);

#endif /* MAIN_H */
