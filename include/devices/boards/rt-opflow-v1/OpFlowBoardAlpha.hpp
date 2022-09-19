#ifndef OPFLOWBOARDALPHA_H
#define OPFLOWBOARDALPHA_H

#include "OpFlowBoard.hpp"

class OpFlowBoardAlpha: public OpFlowBoard {
public: 
  OpFlowBoardAlpha();
  ~OpFlowBoardAlpha();

  uint8_t getDCMIPins(uint32_t *&pins, uint32_t *&pins_groups);
  uint8_t getDCMIPixelBitSize(uint16_t *pixel_bit_size);
  uint8_t getDCMIPixClkPin(uint32_t *pix_clk_pin, uint32_t *pix_clk_pin_group);
  uint8_t getDCMIHSyncPin(uint32_t *hsync_pin, uint32_t *hsync_pin_group);

private:
  uint32_t *_dcmi_pins;
  uint32_t *_dcmi_pins_groups;
  uint16_t _dcmi_pixel_bit_size;
  uint32_t _dcmi_pixclk_pin;
  uint32_t _dcmi_pixclk_pin_group;
  uint32_t _dcmi_hsync_pin;
  uint32_t _dcmi_hsync_pin_group;
  
};

#endif /* OPFLOWBOARDALPHA_H */
