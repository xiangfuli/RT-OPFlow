#include "sad_block_match.hpp"

SADBlockMatcher::SADBlockMatcher(uint32_t img_height, uint32_t img_width, uint32_t win_size)
    : BlockMatcher(win_size, img_height, img_width) {
}

int32_t SADBlockMatcher::computeSADDistance(
  uint8_t *pre_img,
  uint8_t *cur_img,
  uint32_t row1,
  uint32_t col1,
  uint32_t row2,
  uint32_t col2) 
{
  int32_t half_win = this->_win_size / 2;
  int32_t dis = 0;
  for (int32_t row = -half_win; row <= half_win; row++) {
    for (int32_t col = -half_win; col <= half_win; col++) {
      int8_t diff, res;
      diff = *(pre_img + (row1 + row) * this->_img_width + col1 + col)
              - *(cur_img + (row2 + row) * this->_img_width + col2 + col);
      arm_abs_q7(&diff, &res, 1);
      dis += res;
    }
  }
  return dis;
}

// Compute the pixel distance of two images in a window.
// And find the most similar pixel between image1 and image2.
int32_t SADBlockMatcher::match(
  uint8_t *&pre_img,
  uint8_t *&cur_img,
  uint32_t &pre_row,
  uint32_t &pre_col,
  uint32_t &cur_row,
  uint32_t &cur_col) {
    // Find the similar pixel in the window using sad match.
    // Compute sub pixel between two pixels.
    // Use hitogram to check flow distribution to ckeck if it's realiable.
    int32_t half_win = this->_win_size / 2, min_row, min_col;
    int32_t min_dis = -1, dis;
    for (int32_t row = -half_win; row <= half_win; row++) {
        for (int32_t col = -half_win; col <= half_win; col++) {
            if (-1
                == (dis = computeSADDistance(
                        pre_img, cur_img, pre_row, pre_col, cur_row + row,
                        cur_col + col)))
                return -1;
            if (min_dis == -1) {
                min_dis = dis;
                min_row = cur_row + row;
                min_col = cur_col + col;
            } else if (min_dis > dis) {
                min_dis = dis;
                min_row = cur_row + row;
                min_col = cur_col + col;
            }
        }
    }
    cur_row = min_row;
    cur_col = min_col;
    return min_dis;
}