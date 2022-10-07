#include "optical_flow.hpp"
#include "sad_block_match.hpp"


OpticalFlow::OpticalFlow(
  uint32_t img_height,
  uint32_t img_width,
  uint32_t win_size,
  uint32_t step_size,
  uint32_t block_matcher_type)
  : _img_height(img_height)
  , _img_width(img_width)
  , _win_size(win_size)
  , _step_size(step_size) {
  switch (block_matcher_type) {
    case SSD_BLOCK_MATCH:
        _blockMatcher =
            new SADBlockMatcher(img_height, img_width, _win_size);
        break;
    default:
        _blockMatcher =
            new SADBlockMatcher(img_height, img_width, _win_size);
        break;
  }

  _distance_low_threshold = _win_size * _win_size * 2;
  _distance_up_threshold = _win_size * _win_size * 20;

  _shift_arr_size = ((_img_width - 2 * _win_size) / _step_size + 1)
                    * ((_img_height - 2 * _win_size) / _step_size + 1);
  _shift_x = (float *)pvPortMalloc(_shift_arr_size * sizeof(float));
  _shift_y = (float *)pvPortMalloc(_shift_arr_size * sizeof(float));
  _shift_row = (uint16_t *)pvPortMalloc(_shift_arr_size * sizeof(uint16_t));
  _shift_col = (uint16_t *)pvPortMalloc(_shift_arr_size * sizeof(uint16_t));
}

OpticalFlow::~OpticalFlow() {
  vPortFree(_shift_x);
  vPortFree(_shift_y);
  vPortFree(_shift_row);
  vPortFree(_shift_col);
}

// Return optical quality from 0 to 100.
uint32_t OpticalFlow::calFlow(
  uint8_t *&pre_img,
  uint8_t *&cur_img,
  float &mean_shift_x,
  float &mean_shift_y) {
  // For each block find the best matched pixel.
  uint32_t cur_row, cur_col;
  uint32_t row_start = _win_size + 1, row_end = _img_height - _win_size - 2,
      col_start = _win_size + 1, col_end = _img_width - _win_size - 2;
  uint32_t hist_size = 2 * _win_size;
  uint16_t hist_x[hist_size], hist_y[hist_size];
  uint8_t min_dir;
  uint32_t dis;
  _count = 0;
  // Compute sub pixel between two pixels.

  // Compute subpixel shift to improve accuracy.

  // Use hitogram to check flow distribution to ckeck if it's
  // realiable.
  for (uint32_t i = 0; i < hist_size; i++) {
      hist_x[i] = 0;
      hist_y[i] = 0;
  }
  for (uint32_t row = row_start; row < row_end; row += _step_size) {
    for (uint32_t col = col_start; col < col_end; col += _step_size) {
      cur_row = row;
      cur_col = col;
      if (-1 == (dis = _blockMatcher->match(pre_img, cur_img, row, col, cur_row, cur_col)))
          continue;

      // Take its result into consideration if it's sad distance is
      // acceptable.
      if (dis < _distance_up_threshold 
          && dis > _distance_low_threshold) {
        // Get subpixel shift and then set pixel shift histogram
        // considering original pixel and subpixel.
        getSubPixelShift(
            pre_img, cur_img, row, col, cur_row, cur_col, min_dir);

        *(_shift_x + _count) = cur_row - row;
        *(_shift_y + _count) = cur_col - col;
        *(_shift_row + _count) = row;
        *(_shift_col + _count) = col;
        // 0 1 2
        // 3 p 4
        // 5 6 7
        if (min_dir == 2 || min_dir == 4 || min_dir == 7) {
            hist_x[_win_size + 2 * (cur_col - col) + 1]++;
            *(_shift_x + _count) += 0.5f;
        } else if (min_dir == 0 || min_dir == 3 || min_dir == 5) {
            hist_x[_win_size + 2 * (cur_col - col) - 1]++;
            *(_shift_x + _count) -= 0.5f;
        } else {
            hist_x[_win_size + 2 * (cur_col - col)]++;
        }
        if (min_dir == 0 || min_dir == 1 || min_dir == 2) {
            hist_y[_win_size + 2 * (cur_row - row) + 1]++;
            *(_shift_y + _count) += 0.5f;
        } else if (min_dir == 5 || min_dir == 6 || min_dir == 7) {
            hist_y[_win_size + 2 * (cur_row - row) - 1]++;
            *(_shift_y + _count) -= 0.5f;
        } else {
            hist_y[_win_size + 2 * (cur_row - row)]++;
        }
        _count++;
      }
    }
  }

  // check the histogram result. most shift result should concentrate in the
  // same section.
  // If the values located in the near sections of histgram, then only
  // calculate average value of shifts close to each other. Otherwise, count
  // all values and calcuate the average.
  if (_count >= (_img_width - _win_size) / _step_size
                    * (_img_height - _win_size) / _step_size / 4) {
      uint8_t _countx = 0, _county = 0, sum_x = 0, sum_y = 0;
      uint8_t continuous_x[hist_size], continuous_y[hist_size];
      int8_t start_x = -1, end_x = -1, start_y = -1, end_y = -1;
      float quality = 0;
      float average_x = 0, average_y;
      mean_shift_x = 0.f, mean_shift_y = 0.f;
      for (uint32_t i = 0; i < hist_size; i++) {
          continuous_x[i] = 0;
          continuous_y[i] = 0;
          average_x += (float)hist_x[i];
          average_y += (float)hist_y[i];
      }
      average_x /= hist_size;
      average_y /= hist_size;
      for (uint32_t i = 0; i < hist_size; i++) {
          if (hist_x[i] > average_x) { _countx++; }
          if (hist_y[i] > average_y) { _county++; }
      }
      if (_countx > _hist_range || _county > _hist_range) {
          for (uint32_t i = 0; i < _count; i++) {
              mean_shift_x += _shift_x[i];
              mean_shift_y += _shift_y[i];
          }
          mean_shift_x /= _count;
          mean_shift_y /= _count;
          quality = round(_count * 100 / _shift_arr_size);
          return quality;
      }

      for (uint32_t i = 1; i < hist_size; i++) {
          if (hist_x[i] > average_x && hist_x[i - 1] > average_x) {
              continuous_x[i] = 1;
              continuous_x[i - 1] = 1;
          }
          if (hist_y[i] > average_y && hist_y[i - 1] > average_y) {
              continuous_y[i] = 1;
              continuous_y[i - 1] = 1;
          }
      }
      for (uint32_t i = 0; i < hist_size; i++) {
          if (start_x == -1 && hist_x[i] > average_x * 1.5) { start_x = i; }
          if (start_y == -1 && hist_y[i] > average_y * 1.5) { start_y = i; }
      }
      for (uint32_t i = hist_size - 1; i >= 0; i--) {
          if (end_x == -1 && hist_x[i] > average_x * 1.5) { end_x = i; }
          if (end_y == -1 && hist_y[i] > average_y * 1.5) { end_y = i; }
      }
      if (end_x - start_x > _hist_range || end_y - start_y > _hist_range) {
          return -1;
      }

      for (uint32_t i = start_x; i <= end_x; i++) {
          mean_shift_x += hist_x[i] * (i - _win_size);
          sum_x += hist_x[i];
      }
      for (uint32_t i = start_y; i <= end_y; i++) {
          mean_shift_y += hist_y[i] * (i - _win_size);
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
  uint32_t pre_row,
  uint32_t pre_col,
  uint32_t cur_row,
  uint32_t cur_col,
  uint8_t &min_dir) {
  // Get eight blocks of pixel in image 2,
  // Compute each distance at each corner.
  uint32_t dis[_sub_shift_size * _sub_shift_size];
  uint16_t min_dis = 0xFFFF;
  SADBlockMatcher match(_img_height, _img_width, _sub_shift_size);
  for (uint32_t shift_x = -_sub_shift_size / 2; shift_x <= _sub_shift_size / 2;
        shift_x++) {
      for (uint32_t shift_y = -_sub_shift_size / 2; shift_y <= _sub_shift_size / 2;
            shift_y++) {
          dis[(shift_x + 1) * _sub_shift_size + (shift_y + 1)] =
              match.computeSADDistance(
                  pre_img, cur_img, pre_row, pre_col, cur_row + shift_x,
                  cur_col + shift_y);
      }
  }
  // 0 1 2
  // 3 p 4
  // 5 6 7
  for (uint32_t i = 0; i < _sub_shift_size * _sub_shift_size; i++) {
      if (min_dis > dis[i]) {
          min_dis = dis[i];
          min_dir = i;
      }
  }
}