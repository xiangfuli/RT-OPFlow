#ifndef OPTICAL_FLOW_H
#define OPTICAL_FLOW_H


#include "block_match.hpp"
#include "FreeRTOS.h"
#include "stm32f4xx.h"

#define SSD_BLOCK_MATCH 0

class OpticalFlow {
public:
  BlockMatcher *_blockMatcher;
  uint32_t _img_height, _img_width, _win_size, _step_size;
  uint32_t _distance_up_threshold, _distance_low_threshold,
      _sub_shift_size = 3, // Length for subpixel shift
      _hist_range = 10;
  uint32_t _shift_arr_size, _count;
  float *_shift_x, *_shift_y;
  uint16_t *_shift_row, *_shift_col;

  OpticalFlow(
    uint32_t img_height,
    uint32_t img_width,
    uint32_t win_size,
    uint32_t step_size_,
    uint32_t block_matcher_type);
  ~OpticalFlow();

  uint32_t calFlow(
    uint8_t *&pre_img,
    uint8_t *&cur_img,
    float &mean_shift_x,
    float &mean_shift_y);
    
  void getSubPixelShift(
    uint8_t *pre_img,
    uint8_t *cur_img,
    uint32_t pre_row,
    uint32_t pre_col,
    uint32_t cur_row,
    uint32_t cur_col,
    uint8_t &min_dir);
};

#endif /* OPTICAL_FLOW_H */
