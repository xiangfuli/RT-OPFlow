#ifndef DCMI_H
#define DCMI_H

#include "stm32f4xx.h"
#include "stm32f4xx_dcmi.h"
#include "OpFlowBoard.hpp"

#define DCMI_D0_PIN             GPIO_Pin_6
#define DCMI_D0_GPIO_GROUP      GPIOC
#define DCMI_D1_PIN             GPIO_Pin_7
#define DCMI_D1_GPIO_GROUP      GPIOC
#define DCMI_D2_PIN             GPIO_Pin_0
#define DCMI_D2_GPIO_GROUP      GPIOE
#define DCMI_D3_PIN             GPIO_Pin_1
#define DCMI_D3_GPIO_GROUP      GPIOE
#define DCMI_D4_PIN             GPIO_Pin_4
#define DCMI_D4_GPIO_GROUP      GPIOE
#define DCMI_D5_PIN             GPIO_Pin_6
#define DCMI_D5_GPIO_GROUP      GPIOB
#define DCMI_D6_PIN             GPIO_Pin_5
#define DCMI_D6_GPIO_GROUP      GPIOE
#define DCMI_D7_PIN             GPIO_Pin_6
#define DCMI_D7_GPIO_GROUP      GPIOE
#define DCMI_D8_PIN             GPIO_Pin_10
#define DCMI_D8_GPIO_GROUP      GPIOC
#define DCMI_D9_PIN             GPIO_Pin_12
#define DCMI_D9_GPIO_GROUP      GPIOC

#define DCMI_PIXCLK_PIN         GPIO_Pin_6
#define DCMI_PIXCLK_GPIO_GROUP  GPIOA
#define DCMI_HSYNC_PIN          GPIO_Pin_4
#define DCMI_HSYNC_GPIO_GROUP   GPIOA

class STM32DCMI {
public:
    /**
     * @brief Construct a new STM32DCMI object with dcmi pins, pin group and known bit size
     * 
     * @param pins array contains GPIO pins, has the length declared by @ref pixel_bit_size parameter
     * @param pins_groups array contains GPIO pin's groups, has the length declared by @ref pixel_bit_size parameter
     * @param pixel_bit_size number of bits of each pixel
     */
    STM32DCMI();
    ~STM32DCMI();

    void init(OpFlowBoard *board);

private:
    uint8_t _clock_initialized;
    uint8_t _hw_initialized;
    uint8_t _dma_initialized;

    
};



#endif /* DCMI_H */
