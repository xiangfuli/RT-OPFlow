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
  virtual uint8_t get_DCMI_pins(uint16_t *&pins, GPIO_TypeDef *&pins_groups);

  /**
   * @brief Return board dcmi pins configuration
   * 
   * @param pixel_bit_size number of bits of each pixel
   * @return return 1 if set, otherwise return 0
   */
  virtual uint8_t get_DCMI_pixel_bit_size(uint16_t *pixel_bit_size);

  /**
   * @brief set pix_clk_pin parameter with board dcmi PIXCLK pin number
   * 
   * @param pix_clk_pin address of incoming pix_clk_pin parameters
   * @param pix_clk_pin_group pin GPIO group
   * @return uint8_t return 1 if set, otherwise return 0
   */
  virtual uint8_t get_DCMI_pixclk_pin(uint16_t *pix_clk_pin, GPIO_TypeDef *&pix_clk_pin_group);

  /**
   * @brief set hsync parameter with board dcmi HSYNC pin number
   * 
   * @param hsync_pin address of incoming hsync_pin parameters
   * @param hsync_pin_group pin GPIO group
   * @return uint8_t return 1 if set, otherwise return 0
   */
  virtual uint8_t get_DCMI_hsync_pin(uint16_t *hsync_pin, GPIO_TypeDef *&hsync_pin_group);

  /**
   * @brief Get the I2C scl pin in the board
   * 
   * @param scl_pin scl_pin_address
   * @param scl_pin_group scl pin group address
   * @return uint8_t uint8_t return 1 if set, otherwise return 0
   */
  virtual uint8_t get_I2C_scl_pin(uint16_t *scl_pin, GPIO_TypeDef *&scl_pin_group, uint8_t *AF_pin, uint8_t *AF_pin_source);

  /**
   * @brief Get the I2C sda pin in the board
   * 
   * @param sda_pin sda_pin_address
   * @param sda_pin_group sda pin group address
   * @return uint8_t uint8_t return 1 if set, otherwise return 0
   */
  virtual uint8_t get_I2C_sda_pin(uint16_t *sda_pin, GPIO_TypeDef *&sda_pin_group, uint8_t *AF_pin, uint8_t *AF_pin_source);
};

#endif /* OPFLOWBOARD_H */
