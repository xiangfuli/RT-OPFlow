#ifndef OPFLOWBOARD_H
#define OPFLOWBOARD_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"

class OpFlowBoard {
public:
  OpFlowBoard();
  ~OpFlowBoard();

public:
  /**
   * @brief Return board dcmi pins configuration
   * 
   * @param pins array contains GPIO pins, has the length declared by pixel_bit_size
   * @param pins_groups array contains GPIO pins' groups, has the length declared by pixel_bit_size
   * @return return 1 if set, otherwise return 0
   */
  uint8_t getDCMIPins(uint32_t *&pins, uint32_t *&pins_groups);

  /**
   * @brief Return board dcmi pins configuration
   * 
   * @param pixel_bit_size number of bits of each pixel
   * @return return 1 if set, otherwise return 0
   */
  uint8_t getDCMIPixelBitSize(uint16_t *pixel_bit_size);

  /**
   * @brief set pix_clk_pin parameter with board dcmi PIXCLK pin number
   * 
   * @param pix_clk_pin address of incoming pix_clk_pin parameters
   * @param pix_clk_pin_group pin GPIO group
   * @return uint8_t return 1 if set, otherwise return 0
   */
  uint8_t getDCMIPixClkPin(uint32_t *pix_clk_pin, uint32_t *pix_clk_pin_group);

  /**
   * @brief set hsync parameter with board dcmi HSYNC pin number
   * 
   * @param hsync_pin address of incoming hsync_pin parameters
   * @param hsync_pin_group pin GPIO group
   * @return uint8_t return 1 if set, otherwise return 0
   */
  uint8_t getDCMIHSyncPin(uint32_t *hsync_pin, uint32_t *hsync_pin_group);
};

#endif /* OPFLOWBOARD_H */
