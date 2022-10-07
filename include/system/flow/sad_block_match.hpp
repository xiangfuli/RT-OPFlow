#ifndef SAD_BLOCK_MATCH_H
#define SAD_BLOCK_MATCH_H

#include "block_match.hpp"

class SADBlockMatcher: public BlockMatcher {
  public:
    SADBlockMatcher(int img_height, int img_width, int win_size);

    int computeSADDistance(
    uint8_t *pre_img,
    uint8_t *cur_img,
        int row1,
        int col1,
        int row2,
        int col2);

    virtual int match(
    uint8_t *&pre_img,
    uint8_t *&cur_img,
        int &pre_row,
        int &pre_col,
        int &cur_row,
        int &cur_col);
};

#endif /* SAD_BLOCK_MATCH_H */
