#ifndef COMMONS_H
#define COMMONS_H

#include "stm32f4xx.h"

#ifdef __cplusplus 
extern "C" {
#endif


#define  SUCCESS  0
#define  FAILURE  1
#define  SUCCEED( x )  (x == SUCCESS)

/**
 * @brief sleep in us time
 * 
 * @param us time in us unit 
 */
void us_sleep(uint32_t us);

#ifdef __cplusplus 
}
#endif


#endif /* COMMONS_H */
