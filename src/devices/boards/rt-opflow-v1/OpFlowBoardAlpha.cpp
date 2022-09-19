#include "OpFlowBoardAlpha.hpp"

OpFlowBoardAlpha::OpFlowBoardAlpha() {
  this->_dcmi_pixel_bit_size = 10;
  this->_dcmi_pins = new uint32_t[this->_dcmi_pixel_bit_size];
  this->_dcmi_pins_groups = new uint32_t[this->_dcmi_pixel_bit_size];
  this->_dcmi_pins[0] =           GPIO_Pin_6;                   
  this->_dcmi_pins_groups[0] =    GPIOC_BASE;
  this->_dcmi_pins[1] =           GPIO_Pin_7;
  this->_dcmi_pins_groups[1] =    GPIOC_BASE;
  this->_dcmi_pins[2] =           GPIO_Pin_0;
  this->_dcmi_pins_groups[2] =    GPIOE_BASE;
  this->_dcmi_pins[3] =           GPIO_Pin_1;
  this->_dcmi_pins_groups[3] =    GPIOE_BASE;
  this->_dcmi_pins[4] =           GPIO_Pin_4;
  this->_dcmi_pins_groups[4] =    GPIOE_BASE;
  this->_dcmi_pins[5] =           GPIO_Pin_6;
  this->_dcmi_pins_groups[5] =    GPIOB_BASE;
  this->_dcmi_pins[6] =           GPIO_Pin_5;
  this->_dcmi_pins_groups[6] =    GPIOE_BASE;
  this->_dcmi_pins[7] =           GPIO_Pin_6;
  this->_dcmi_pins_groups[7] =    GPIOE_BASE;
  this->_dcmi_pins[8] =           GPIO_Pin_10;
  this->_dcmi_pins_groups[8] =    GPIOC_BASE;
  this->_dcmi_pins[9] =           GPIO_Pin_12;
  this->_dcmi_pins_groups[9] =    GPIOC_BASE;
  this->_dcmi_pixclk_pin =        GPIO_Pin_6;
  this->_dcmi_pixclk_pin_group =  GPIOA_BASE;
  this->_dcmi_hsync_pin =         GPIO_Pin_4;
  this->_dcmi_hsync_pin_group =   GPIOA_BASE;
}
  
OpFlowBoardAlpha::~OpFlowBoardAlpha() {

}

uint8_t OpFlowBoardAlpha::getDCMIPins(uint32_t *&pins, uint32_t *&pins_groups)  {
  pins = this->_dcmi_pins;
  pins_groups = this->_dcmi_pins_groups;
  return 1;
}

uint8_t OpFlowBoardAlpha::getDCMIPixelBitSize(uint16_t *pixel_bit_size) {
  *pixel_bit_size = this->_dcmi_pixel_bit_size;
  return 1;
}

uint8_t OpFlowBoardAlpha::getDCMIPixClkPin(uint32_t *pix_clk_pin, uint32_t *dcmi_pixclk_pin_group) {
  *pix_clk_pin = this->_dcmi_pixclk_pin;
  *dcmi_pixclk_pin_group = this->_dcmi_pixclk_pin_group;
  return 1;
}

uint8_t OpFlowBoardAlpha::getDCMIHSyncPin(uint32_t *hsync_pin, uint32_t *hsync_pin_group) {
  *hsync_pin = this->_dcmi_hsync_pin;
  *hsync_pin_group = this->_dcmi_hsync_pin_group;
  return 1;
}