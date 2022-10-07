#include "sad_block_match.hpp"

SADBlockMatcher::SADBlockMatcher(int img_height, int img_width, int win_size)
    : BlockMatcher(win_size, img_height, img_width) {
}

int SADBlockMatcher::computeSADDistance(
    uint8_t *pre_img,
    uint8_t *cur_img,
    int row1,
    int col1,
    int row2,
    int col2) {
    int half_win = win_size_ / 2;
    int dis = 0;
    for (int row = -half_win; row <= half_win; row++) {
        for (int col = -half_win; col <= half_win; col++) {
          int32_t res = *(pre_img + (row1 + row) * img_width_ + col1 + col) - *(cur_img + (row2 + row) * img_width_ + col2 + col);
          if (res > 0) {
            dis += res;
          } else {
            dis += -res;
          }
        }
    }
    return dis;
}

// Compute the pixel distance of two images in a window.
// And find the most similar pixel between image1 and image2.
int SADBlockMatcher::match(
    uint8_t *&pre_img,
    uint8_t *&cur_img,
    int &pre_row,
    int &pre_col,
    int &cur_row,
    int &cur_col) {
    // Find the similar pixel in the window using sad match.
    // Compute sub pixel between two pixels.
    // Use hitogram to check flow distribution to ckeck if it's realiable.
    int half_win = win_size_ / 2, min_row, min_col;
    int min_dis = -1, dis;
    for (int row = -half_win; row <= half_win; row++) {
        for (int col = -half_win; col <= half_win; col++) {
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