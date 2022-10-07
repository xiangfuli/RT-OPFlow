#include "optical_flow.hpp"
#include "sad_block_match.hpp"

OpticalFlow::OpticalFlow(
    int img_height,
    int img_width,
    int win_size,
    int step_size,
    int block_matcher_type)
    : img_height_(img_height)
    , img_width_(img_width)
    , win_size_(win_size)
    , step_size_(step_size) {
    switch (block_matcher_type) {
        case SSD_BLOCK_MATCH:
            blockMatcher_ =
                new SADBlockMatcher(img_height, img_width, win_size_);
            break;
        default:
            blockMatcher_ =
                new SADBlockMatcher(img_height, img_width, win_size_);
            break;
    }

    distance_low_threshold_ = win_size_ * win_size_ * 2;
    distance_up_threshold_ = win_size_ * win_size_ * 20;

    shift_arr_size_ = ((img_width_ - 2 * win_size_) / step_size_ + 1)
                      * ((img_height_ - 2 * win_size_) / step_size_ + 1);
    shift_x_ = (float *)pvPortMalloc(shift_arr_size_ * sizeof(float));
    shift_y_ = (float *)pvPortMalloc(shift_arr_size_ * sizeof(float));
    shift_row_ = (uint16_t *)pvPortMalloc(shift_arr_size_ * sizeof(uint16_t));
    shift_col_ = (uint16_t *)pvPortMalloc(shift_arr_size_ * sizeof(uint16_t));
}

OpticalFlow::~OpticalFlow() {
    vPortFree(shift_x_);
    vPortFree(shift_y_);
    vPortFree(shift_row_);
    vPortFree(shift_col_);
}

// Return optical quality from 0 to 100.
int OpticalFlow::calFlow(
    uint8_t *pre_img,
    uint8_t *cur_img,
    float &mean_shift_x,
    float &mean_shift_y) {
    // For each block find the best matched pixel.
    int cur_row, cur_col;
    int row_start = win_size_ + 1, row_end = img_height_ - win_size_ - 2,
        col_start = win_size_ + 1, col_end = img_width_ - win_size_ - 2;
    int hist_size = 2 * win_size_;
    uint16_t hist_x[hist_size], hist_y[hist_size];
    uint8_t min_dir;
    int dis;
    count_ = 0;
    // Compute sub pixel between two pixels.

    // Compute subpixel shift to improve accuracy.

    // Use hitogram to check flow distribution to ckeck if it's
    // realiable.
    for (int i = 0; i < hist_size; i++) {
        hist_x[i] = 0;
        hist_y[i] = 0;
    }
    for (int row = row_start; row < row_end; row += step_size_) {
        for (int col = col_start; col < col_end; col += step_size_) {
            cur_row = row;
            cur_col = col;
            if (-1
                == (dis = blockMatcher_->match(
                        pre_img, cur_img, row, col, cur_row, cur_col)))
                continue;

            // Take its result into consideration if it's sad distance is
            // acceptable.
            if (dis < distance_up_threshold_ && dis > distance_low_threshold_) {
                // Get subpixel shift and then set pixel shift histogram
                // considering original pixel and subpixel.
                getSubPixelShift(
                    pre_img, cur_img, row, col, cur_row, cur_col, min_dir);

                *(shift_x_ + count_) = cur_row - row;
                *(shift_y_ + count_) = cur_col - col;
                *(shift_row_ + count_) = row;
                *(shift_col_ + count_) = col;
                // 0 1 2
                // 3 p 4
                // 5 6 7
                if (min_dir == 2 || min_dir == 4 || min_dir == 7) {
                    hist_x[win_size_ + 2 * (cur_col - col) + 1]++;
                    *(shift_x_ + count_) += 0.5f;
                } else if (min_dir == 0 || min_dir == 3 || min_dir == 5) {
                    hist_x[win_size_ + 2 * (cur_col - col) - 1]++;
                    *(shift_x_ + count_) -= 0.5f;
                } else {
                    hist_x[win_size_ + 2 * (cur_col - col)]++;
                }
                if (min_dir == 0 || min_dir == 1 || min_dir == 2) {
                    hist_y[win_size_ + 2 * (cur_row - row) + 1]++;
                    *(shift_y_ + count_) += 0.5f;
                } else if (min_dir == 5 || min_dir == 6 || min_dir == 7) {
                    hist_y[win_size_ + 2 * (cur_row - row) - 1]++;
                    *(shift_y_ + count_) -= 0.5f;
                } else {
                    hist_y[win_size_ + 2 * (cur_row - row)]++;
                }
                count_++;
            }
        }
    }

    // check the histogram result. most shift result should concentrate in the
    // same section.
    // If the values located in the near sections of histgram, then only
    // calculate average value of shifts close to each other. Otherwise, count
    // all values and calcuate the average.
    if (count_ >= (img_width_ - win_size_) / step_size_
                      * (img_height_ - win_size_) / step_size_ / 4) {
        uint8_t count_x = 0, count_y = 0, sum_x = 0, sum_y = 0;
        uint8_t continuous_x[hist_size], continuous_y[hist_size];
        int8_t start_x = -1, end_x = -1, start_y = -1, end_y = -1;
        float quality = 0;
        float average_x = 0, average_y;
        mean_shift_x = 0.f, mean_shift_y = 0.f;
        for (int i = 0; i < hist_size; i++) {
            continuous_x[i] = 0;
            continuous_y[i] = 0;
            average_x += (float)hist_x[i];
            average_y += (float)hist_y[i];
        }
        average_x /= hist_size;
        average_y /= hist_size;
        for (int i = 0; i < hist_size; i++) {
            if (hist_x[i] > average_x) { count_x++; }
            if (hist_y[i] > average_y) { count_y++; }
        }
        if (count_x > hist_range_ || count_y > hist_range_) {
            for (int i = 0; i < count_; i++) {
                mean_shift_x += shift_x_[i];
                mean_shift_y += shift_y_[i];
            }
            mean_shift_x /= count_;
            mean_shift_y /= count_;
            quality = round(count_ * 100 / shift_arr_size_);
            return quality;
        }

        for (int i = 1; i < hist_size; i++) {
            if (hist_x[i] > average_x && hist_x[i - 1] > average_x) {
                continuous_x[i] = 1;
                continuous_x[i - 1] = 1;
            }
            if (hist_y[i] > average_y && hist_y[i - 1] > average_y) {
                continuous_y[i] = 1;
                continuous_y[i - 1] = 1;
            }
        }
        for (int i = 0; i < hist_size; i++) {
            if (start_x == -1 && hist_x[i] > average_x * 1.5) { start_x = i; }
            if (start_y == -1 && hist_y[i] > average_y * 1.5) { start_y = i; }
        }
        for (int i = hist_size - 1; i >= 0; i--) {
            if (end_x == -1 && hist_x[i] > average_x * 1.5) { end_x = i; }
            if (end_y == -1 && hist_y[i] > average_y * 1.5) { end_y = i; }
        }
        if (end_x - start_x > hist_range_ || end_y - start_y > hist_range_) {
            return -1;
        }

        for (int i = start_x; i <= end_x; i++) {
            mean_shift_x += hist_x[i] * (i - win_size_);
            sum_x += hist_x[i];
        }
        for (int i = start_y; i <= end_y; i++) {
            mean_shift_y += hist_y[i] * (i - win_size_);
            sum_y += hist_y[i];
        }
        mean_shift_x = round(mean_shift_x / 2 / sum_x);
        mean_shift_y = round(mean_shift_y / 2 / sum_y);

        quality = sum_x / (average_x * hist_size) * 100
                  + sum_x / (average_y * hist_size) * 100;
        quality /= 2;
        return round(quality);
    }
    return -1;
}

void OpticalFlow::getSubPixelShift(
    uint8_t *pre_img,
    uint8_t *cur_img,
    int pre_row,
    int pre_col,
    int cur_row,
    int cur_col,
    uint8_t &min_dir) {
    // Get eight blocks of pixel in image 2,
    // Compute each distance at each corner.
    int dis[sub_shift_size_ * sub_shift_size_];
    uint16_t min_dis = 0xFFFF;
    SADBlockMatcher match(img_height_, img_width_, sub_shift_size_);
    for (int shift_x = -sub_shift_size_ / 2; shift_x <= sub_shift_size_ / 2;
         shift_x++) {
        for (int shift_y = -sub_shift_size_ / 2; shift_y <= sub_shift_size_ / 2;
             shift_y++) {
            dis[(shift_x + 1) * sub_shift_size_ + (shift_y + 1)] =
                match.computeSADDistance(
                    pre_img, cur_img, pre_row, pre_col, cur_row + shift_x,
                    cur_col + shift_y);
        }
    }
    // 0 1 2
    // 3 p 4
    // 5 6 7
    for (int i = 0; i < sub_shift_size_ * sub_shift_size_; i++) {
        if (min_dis > dis[i]) {
            min_dis = dis[i];
            min_dir = i;
        }
    }
}