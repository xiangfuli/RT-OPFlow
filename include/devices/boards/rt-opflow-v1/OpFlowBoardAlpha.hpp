#ifndef OPFLOWBOARDALPHA_H
#define OPFLOWBOARDALPHA_H

#include "OpFlowBoard.hpp"

class OpFlowBoardAlpha: public OpFlowBoard {
public: 
  OpFlowBoardAlpha();
  ~OpFlowBoardAlpha();

  uint8_t get_DCMI_pins(uint16_t *&pins, GPIO_TypeDef **&pins_groups);
  uint8_t get_DCMI_pixel_bit_size(uint16_t *pixel_bit_size);
  uint8_t get_DCMI_pixclk_pin(uint16_t *pix_clk_pin, GPIO_TypeDef *&pix_clk_pin_group);
  uint8_t get_DCMI_hsync_pin(uint16_t *hsync_pin, GPIO_TypeDef *&hsync_pin_group);

  uint8_t get_I2C_scl_pin(uint16_t *scl_pin, GPIO_TypeDef *&scl_pin_group, uint8_t *AF_pin, uint8_t *AF_pin_source);
  uint8_t get_I2C_sda_pin(uint16_t *sda_pin, GPIO_TypeDef *&sda_pin_group, uint8_t *AF_pin, uint8_t *AF_pin_source);

private:
  uint16_t *_dcmi_pins;
  GPIO_TypeDef **_dcmi_pins_groups;
  uint16_t _dcmi_pixel_bit_size;
  uint16_t _dcmi_pixclk_pin;
  GPIO_TypeDef *_dcmi_pixclk_pin_group;
  uint16_t _dcmi_hsync_pin;
  GPIO_TypeDef *_dcmi_hsync_pin_group;
};

#endif /* OPFLOWBOARDALPHA_H */
