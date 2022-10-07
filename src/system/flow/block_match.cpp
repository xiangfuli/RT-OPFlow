#include "block_match.hpp"

BlockMatcher::BlockMatcher(int win_size, int img_height, int img_width): win_size_(win_size), img_height_(img_height), img_width_(img_width){
    if(win_size % 2 != 1){
        win_size_ = win_size  + 1;
    }
}