#include "OpFlowBoardAlpha.hpp"

OpFlowBoardAlpha::OpFlowBoardAlpha() {
  this->_dcmi_pixel_bit_size = 10;
  this->_dcmi_pins = new uint16_t[this->_dcmi_pixel_bit_size];
  this->_dcmi_pins_groups = new GPIO_TypeDef *[this->_dcmi_pixel_bit_size];
  this->_dcmi_pins[0] =           GPIO_Pin_6;                   
  this->_dcmi_pins_groups[0] =    GPIOC;
  this->_dcmi_pins[1] =           GPIO_Pin_7;
  this->_dcmi_pins_groups[1] =    GPIOC;
  this->_dcmi_pins[2] =           GPIO_Pin_0;
  this->_dcmi_pins_groups[2] =    GPIOE;
  this->_dcmi_pins[3] =           GPIO_Pin_1;
  this->_dcmi_pins_groups[3] =    GPIOE;
  this->_dcmi_pins[4] =           GPIO_Pin_4;
  this->_dcmi_pins_groups[4] =    GPIOE;
  this->_dcmi_pins[5] =           GPIO_Pin_6;
  this->_dcmi_pins_groups[5] =    GPIOB;
  this->_dcmi_pins[6] =           GPIO_Pin_5;
  this->_dcmi_pins_groups[6] =    GPIOE;
  this->_dcmi_pins[7] =           GPIO_Pin_6;
  this->_dcmi_pins_groups[7] =    GPIOE;
  this->_dcmi_pins[8] =           GPIO_Pin_10;
  this->_dcmi_pins_groups[8] =    GPIOC;
  this->_dcmi_pins[9] =           GPIO_Pin_12;
  this->_dcmi_pins_groups[9] =    GPIOC;
  this->_dcmi_pixclk_pin =        GPIO_Pin_6;
  this->_dcmi_pixclk_pin_group =  GPIOA;
  this->_dcmi_hsync_pin =         GPIO_Pin_4;
  this->_dcmi_hsync_pin_group =   GPIOA;


}
  
OpFlowBoardAlpha::~OpFlowBoardAlpha() {

}

uint8_t OpFlowBoardAlpha::get_DCMI_pins(uint16_t *&pins, GPIO_TypeDef **&pins_groups)  {
  pins = this->_dcmi_pins;
  pins_groups = this->_dcmi_pins_groups;
  return 1;
}

uint8_t OpFlowBoardAlpha::get_DCMI_pixel_bit_size(uint16_t *pixel_bit_size) {
  *pixel_bit_size = this->_dcmi_pixel_bit_size;
  return 1;
}

uint8_t OpFlowBoardAlpha::get_DCMI_pixclk_pin(uint16_t *pix_clk_pin, GPIO_TypeDef *&pix_clk_pin_group) {
  *pix_clk_pin = this->_dcmi_pixclk_pin;
  pix_clk_pin_group = this->_dcmi_pixclk_pin_group;
  return 1;
}

uint8_t OpFlowBoardAlpha::get_DCMI_hsync_pin(uint16_t *hsync_pin, GPIO_TypeDef *&hsync_pin_group) {
  *hsync_pin = this->_dcmi_hsync_pin;
  hsync_pin_group = this->_dcmi_hsync_pin_group;
  return 1;
}

uint8_t  OpFlowBoardAlpha::get_I2C_scl_pin(uint16_t *scl_pin, GPIO_TypeDef *&scl_pin_group, uint8_t *AF_pin, uint8_t *AF_pin_source) {

  uint8_t sda_pin_AF_source, sda_pin_AF;
  uint16_t sda_pin;
  GPIO_TypeDef *sda_pin_group;
  this->get_I2C_sda_pin(&sda_pin, sda_pin_group, &sda_pin_AF, &sda_pin_AF_source);

  *scl_pin = GPIO_Pin_10;
  scl_pin_group = GPIOB;
  *AF_pin_source = GPIO_PinSource10;
  *AF_pin = GPIO_AF_I2C2;
  return 1;
}

uint8_t  OpFlowBoardAlpha::get_I2C_sda_pin(uint16_t *sda_pin, GPIO_TypeDef *&sda_pin_group, uint8_t *AF_pin, uint8_t *AF_pin_source) {
  *sda_pin = GPIO_Pin_11;
  sda_pin_group = GPIOB;
  *AF_pin_source = GPIO_PinSource11;
  *AF_pin = GPIO_AF_I2C2;
  return 1;
}