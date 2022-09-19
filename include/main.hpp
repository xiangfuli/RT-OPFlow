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

#include "SensorsManager.hpp"

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
int main(int ac, char** av);

#endif /* MAIN_H */
