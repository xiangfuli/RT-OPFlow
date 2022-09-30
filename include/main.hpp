#ifndef MAIN_H
#define MAIN_H
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "usbd_core.h"
#include "usbd_cdc_core.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "usbd_usr.h"

#include "i2c_host.hpp"


#include "OpFlowBoard.hpp"
#include "OpFlowBoardAlpha.hpp"
#include "system_manager.hpp"
#include "config_manager.hpp"
#include "storage_manager.hpp"
#include "sensor_manager.hpp"
#include "message_manager.hpp"

/**
 * @brief Interrupt handlers
 * 
 */

extern "C" {
  void I2C2_ER_IRQHandler();
  void DMA2_Stream1_IRQHander(void);
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
