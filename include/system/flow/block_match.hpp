#ifndef BLOCK_MATCH_H
#define BLOCK_MATCH_H


#include "stm32f4xx.h"
#include "arm_math.h"


class BlockMatcher {
  public:
    int win_size_,img_height_, img_width_;

    BlockMatcher(int win_size, int img_height, int img_width);

    virtual int match(
    uint8_t *&pre_img,
    uint8_t *&cur_img,
        int &pre_row,
        int &pre_col,
        int &cur_row,
        int &cur_col) = 0;
};

#endif /* BLOCK_MATCH_H */
