#include "block_match.hpp"

BlockMatcher::BlockMatcher(uint32_t win_size, uint32_t img_height, uint32_t img_width)
:_win_size(win_size), 
_img_height(img_height), 
_img_width(img_width){
  if(win_size % 2 != 1){
      this->_win_size = win_size  + 1;
  }
}