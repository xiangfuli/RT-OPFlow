#ifndef MT9V034_H
#define MT9V034_H

#include "stm32f4xx.h"
#include "i2c_host.hpp"
#include "commons.h"


#define MT9V034_I2C_DEVICE_ADDRESS                               0x48
/**
 * Register address
 */
/** Common registers **/
#define MT9V034_CHIP_CONTROL_REGISTER                            0x07
#define MT9V034_SENSOR_TYPE_REGISTER                             0x0F
#define MT9V034_LED_OUT_REGISTER                                 0x1B
#define MT9V034_ADC_COMPANDING_MODE_REGISTER                     0x1C
#define MT9V034_AEC_AGC_DESIRED_BIN_REGISTER                     0xA5
#define MT9V034_AEC_LOW_PASS_FILTER_REGISTER                     0xA8
#define MT9V034_AGC_LOW_PASS_FILTER_REGISTER                     0xAA
#define MT9V034_AEC_AGC_BIN_DIFF_TRESHOLD_REGISTER               0xAE
#define MT9V034_AEC_AGC_ENABLE_REGISTER                          0xAF

/** Context A **/         
#define MT9V034_CONTEXT_A_ROW_START_REGISTER                     0x01
#define MT9V034_CONTEXT_A_COL_START_REGISTER                     0x02
#define MT9V034_CONTEXT_A_WINDOWS_HEIGHT_REGISTER                0x03
#define MT9V034_CONTEXT_A_WINDOWS_WIDTH_REGISTER                 0x04
#define MT9V034_CONTEXT_A_HOR_BLANKING_REGISTER                  0x05
#define MT9V034_CONTEXT_A_VER_BLANKING_REGISTER                  0x06
#define MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_1_REGISTER        0x08
#define MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_2_REGISTER        0x09
#define MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_CTRL_REGISTER     0x0A
#define MT9V034_CONTEXT_A_READ_MODE_REGISTER                     0x0D

/** Context B **/
#define MT9V034_CONTEXT_B_ROW_START_REGISTER                     0xC9
#define MT9V034_CONTEXT_B_COL_START_REGISTER                     0xCA
#define MT9V034_CONTEXT_B_WINDOWS_HEIGHT_REGISTER                0xCB
#define MT9V034_CONTEXT_B_WINDOWS_WIDTH_REGISTER                 0xCC
#define MT9V034_CONTEXT_B_HOR_BLANKING_REGISTER                  0xCD
#define MT9V034_CONTEXT_B_VER_BLANKING_REGISTER                  0xCE
#define MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_1_REGISTER        0xCF
#define MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_2_REGISTER        0xD0
#define MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_CTRL_REGISTER     0xD1
#define MT9V034_CONTEXT_B_READ_MODE_REGISTER                     0x0E

/**
 * Default values for register
 */
/** Common registers **/
#define MT9V034_CHIP_CONTROL_REGISTER_DEFAULT_VALUE                            0x0388
#define MT9V034_SENSOR_TYPE_REGISTER_DEFAULT_VALUE                             0x0101
#define MT9V034_LED_OUT_REGISTER_DEFAULT_VALUE                                 0x0000
#define MT9V034_ADC_COMPANDING_MODE_REGISTER_DEFAULT_VALUE                     0x0202
#define MT9V034_AEC_AGC_DESIRED_BIN_REGISTER_DEFAULT_VALUE                     0x003A
#define MT9V034_AEC_LOW_PASS_FILTER_REGISTER_DEFAULT_VALUE                     0x0001
#define MT9V034_AGC_LOW_PASS_FILTER_REGISTER_DEFAULT_VALUE                     0x0002
#define MT9V034_AEC_AGC_BIN_DIFF_TRESHOLD_REGISTER_DEFAULT_VALUE               0x000A
#define MT9V034_AEC_AGC_ENABLE_REGISTER_DEFAULT_VALUE                          0x0303

/** Context A **/         
#define MT9V034_CONTEXT_A_ROW_START_REGISTER_DEFAULT_VALUE                     0x0001
#define MT9V034_CONTEXT_A_COL_START_REGISTER_DEFAULT_VALUE                     0x0004
#define MT9V034_CONTEXT_A_WINDOWS_HEIGHT_REGISTER_DEFAULT_VALUE                0x01E0
#define MT9V034_CONTEXT_A_WINDOWS_WIDTH_REGISTER_DEFAULT_VALUE                 0x02F0
#define MT9V034_CONTEXT_A_HOR_BLANKING_REGISTER_DEFAULT_VALUE                  0x005B
#define MT9V034_CONTEXT_A_VER_BLANKING_REGISTER_DEFAULT_VALUE                  0x000A
#define MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_1_REGISTER_DEFAULT_VALUE        0x01BB
#define MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_2_REGISTER_DEFAULT_VALUE        0x01D9
#define MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_CTRL_REGISTER_DEFAULT_VALUE     0x0164
#define MT9V034_CONTEXT_A_READ_MODE_REGISTER_DEFAULT_VALUE                     0x006A

/** Context B **/
#define MT9V034_CONTEXT_B_ROW_START_REGISTER_DEFAULT_VALUE                     0x0001
#define MT9V034_CONTEXT_B_COL_START_REGISTER_DEFAULT_VALUE                     0x0004
#define MT9V034_CONTEXT_B_WINDOWS_HEIGHT_REGISTER_DEFAULT_VALUE                0x01E0
#define MT9V034_CONTEXT_B_WINDOWS_WIDTH_REGISTER_DEFAULT_VALUE                 0x02F0
#define MT9V034_CONTEXT_B_HOR_BLANKING_REGISTER_DEFAULT_VALUE                  0x005B
#define MT9V034_CONTEXT_B_VER_BLANKING_REGISTER_DEFAULT_VALUE                  0x000A
#define MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_1_REGISTER_DEFAULT_VALUE        0x01BB
#define MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_2_REGISTER_DEFAULT_VALUE        0x01D9
#define MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_CTRL_REGISTER_DEFAULT_VALUE     0x0164
#define MT9V034_CONTEXT_B_READ_MODE_REGISTER_DEFAULT_VALUE                     0x0060


#define MT9V034_CAMERA_FULL_WIDTH                                              752
#define MT9V034_CAMERA_FULL_HEIGHT                                             480

#define MT9V034_OF_MODE_DMA_BUFFER_SIZE                                        MT9V034_CAMERA_FULL_WIDTH*MT9V034_CAMERA_FULL_HEIGHT/16
#define MT9V034_HR_MODE_DMA_BUFFER_SIZE                                        MT9V034_OF_MODE_DMA_BUFFER_SIZE

typedef enum {
  HR_MODE,
  FLOW_MODE
} MT9V034_mode;

class MT9V034 {
public:
  MT9V034(I2CHost *i2c_host);
  ~MT9V034();

  uint8_t init(uint8_t *buffer0, uint8_t *buffer1);

  uint8_t start_image_capture();
  uint8_t stop_image_capture();

  uint8_t _common_register_inited;

  uint8_t dcmi_dma_it_handler();

  uint32_t get_numbers_of_received_images();

private:
  uint8_t rst_ctrl_init();
  uint8_t sys_clk_init();
  uint8_t dcmi_pin_init();
  uint8_t dcmi_dma_init();
  uint8_t dcmi_dma_enable();
  uint8_t dcmi_dma_disable();
  uint8_t common_register_init();
  uint8_t context_register_init();
  uint8_t select_mode(uint8_t mode);

  uint8_t swap_buffer();

  /**
   * @brief reset the camera state to FLOW_Mode and reset all the register
   * 
   * @return uint8_t always return 1 as successfully executed.
   */
  uint8_t reset();

  /**
   * @brief I2C operations
   * 
   */
  uint8_t write_register(uint8_t address, uint16_t value);
  uint8_t read_register(uint8_t address, uint8_t *value);

  MT9V034_mode _mode;
  I2CHost *_i2c_host;

  uint32_t _numbers_of_received_images;

  // dma transmission buffer
  uint8_t _current_buffer_index;
  uint8_t *_dma_buffer_0;
  uint8_t *_dma_buffer_1;
  
  uint8_t _hr_mode_dma_transmission_times_index;
};

extern "C" {
  void MT9V034_DCMI_DMA_it_callback(MT9V034 *mt9v034);
}

#endif /* MT9V034_H */

