#ifndef SAD_BLOCK_MATCH_H
#define SAD_BLOCK_MATCH_H


#include "block_match.hpp"

class SADBlockMatcher : public BlockMatcher {
public:
  SADBlockMatcher(uint32_t img_height, uint32_t img_width, uint32_t win_size);

  int32_t computeSADDistance(
    uint8_t *pre_img,
    uint8_t *cur_img,
    uint32_t row1,
    uint32_t col1,
    uint32_t row2,
    uint32_t col2);

  int32_t match(
    uint8_t *&pre_img,
    uint8_t *&cur_img,
    uint32_t &pre_row,
    uint32_t &pre_col,
    uint32_t &cur_row,
    uint32_t &cur_col);
};

#endif /* SAD_BLOCK_MATCH_H */
