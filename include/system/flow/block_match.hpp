#ifndef BLOCK_MATCH_H
#define BLOCK_MATCH_H


#include "stm32f4xx.h"
#include "arm_math.h"

using namespace std;

class BlockMatcher {
public:
  uint32_t _win_size, _img_height, _img_width;

  BlockMatcher(uint32_t win_size, uint32_t img_height, uint32_t img_width);

  virtual int32_t match(
    uint8_t *&pre_img,
    uint8_t *&cur_img,
    uint32_t &pre_row,
    uint32_t &pre_col,
    uint32_t &cur_row,
    uint32_t &cur_col) = 0;
};

#endif /* BLOCK_MATCH_H */
