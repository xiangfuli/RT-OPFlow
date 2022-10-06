#include "mt9v034.hpp"


MT9V034::MT9V034(I2CHost *i2c_host):
_i2c_host(i2c_host),
_current_buffer_index(0),
_mode(FLOW_MODE),
_initialized(false)
{
  this->_i2c_host = i2c_host;
}
MT9V034::~MT9V034() {
}

uint8_t MT9V034::init(uint8_t *buffer0, uint8_t *buffer1) {
  this->_dma_buffer_0 = buffer0;
  this->_dma_buffer_1 = buffer1;

  uint8_t init_res = 0;
  init_res = this->sys_clk_init();
  init_res = init_res && this->rst_ctrl_init();
  init_res = init_res && this->dcmi_pin_init();

  // reset chip and then set registers
  init_res = init_res && this->reset();

  // start FLOW_MODE as default
  init_res = init_res && this->common_register_init();

  init_res = init_res && this->context_register_init();

  init_res = init_res && this->select_mode(FLOW_MODE);

  this->_initialized = true;

  return init_res;
}

uint8_t MT9V034::sys_clk_init() {
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	/* GPIOC clock enable */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	/* GPIOC Configuration:  TIM3 CH3 (PC8)  */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* Connect TIM3 pins to AF2 */;
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);

  /**
   * Time base configuration 
   * chip will run in 21Mhz sysclk frequency - 84 / (3 + 1) * (0 + 1) 
   */
	TIM_TimeBaseStructure.TIM_Period = 3;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 2;

	TIM_OC3Init(TIM3, &TIM_OCInitStructure);

	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);

  /* Initialize GPIOs for EXPOSURE and STANDBY lines of the camera */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_2 | GPIO_Pin_3);

  return 1;
}

uint8_t MT9V034::reset() {
  // use internal 0x0C software reset register to reset all register value
  this->write_register(0x0C, 1);
  us_sleep(100);
  return 1;
}

uint8_t MT9V034::rst_ctrl_init() {
  /**
   * @brief initialize RES_CTRL pin 
   */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

  // remain RST_CTRL at low
  GPIO_ResetBits(GPIOD, GPIO_Pin_10);

  return 1;
}

uint8_t MT9V034::common_register_init() {
  uint8_t res = 0;

  res = this->write_register(0xFE, 0xBEEF);
  res = res && this->write_register(MT9V034_CHIP_CONTROL_REGISTER, MT9V034_CHIP_CONTROL_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_SENSOR_TYPE_REGISTER, MT9V034_SENSOR_TYPE_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_LED_OUT_REGISTER, MT9V034_LED_OUT_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_AEC_AGC_DESIRED_BIN_REGISTER, MT9V034_AEC_AGC_DESIRED_BIN_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_AEC_AGC_BIN_DIFF_TRESHOLD_REGISTER, MT9V034_AEC_AGC_BIN_DIFF_TRESHOLD_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_AEC_LOW_PASS_FILTER_REGISTER, MT9V034_AEC_LOW_PASS_FILTER_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_AGC_LOW_PASS_FILTER_REGISTER, MT9V034_AGC_LOW_PASS_FILTER_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_AEC_AGC_ENABLE_REGISTER, MT9V034_AEC_AGC_ENABLE_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_ADC_COMPANDING_MODE_REGISTER, MT9V034_ADC_COMPANDING_MODE_REGISTER_DEFAULT_VALUE);

  // reset register
  res = this->write_register(0x0C, 0x0001);
  return res;
}

uint8_t MT9V034::context_register_init() {
  uint8_t res = 1;
  /**
   * Context A register
   */
  res = this->write_register(MT9V034_CONTEXT_A_ROW_START_REGISTER, MT9V034_CONTEXT_A_ROW_START_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_A_COL_START_REGISTER, MT9V034_CONTEXT_A_COL_START_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_A_WINDOWS_HEIGHT_REGISTER, MT9V034_CONTEXT_A_WINDOWS_HEIGHT_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_A_WINDOWS_WIDTH_REGISTER, MT9V034_CONTEXT_A_WINDOWS_WIDTH_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_A_VER_BLANKING_REGISTER, MT9V034_CONTEXT_A_VER_BLANKING_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_A_HOR_BLANKING_REGISTER, MT9V034_CONTEXT_A_HOR_BLANKING_REGISTER_DEFAULT_VALUE);  
  res = res && this->write_register(MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_1_REGISTER, MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_1_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_2_REGISTER, MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_2_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_CTRL_REGISTER, MT9V034_CONTEXT_A_COARSE_SHUTTER_WIDTH_CTRL_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_A_READ_MODE_REGISTER, MT9V034_CONTEXT_A_READ_MODE_REGISTER_DEFAULT_VALUE);

/**
  * Context B register
  */
  res = res && this->write_register(MT9V034_CONTEXT_B_ROW_START_REGISTER, MT9V034_CONTEXT_B_ROW_START_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_B_COL_START_REGISTER, MT9V034_CONTEXT_B_COL_START_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_B_WINDOWS_HEIGHT_REGISTER, MT9V034_CONTEXT_B_WINDOWS_HEIGHT_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_B_WINDOWS_WIDTH_REGISTER, MT9V034_CONTEXT_B_WINDOWS_WIDTH_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_B_HOR_BLANKING_REGISTER, MT9V034_CONTEXT_B_HOR_BLANKING_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_B_VER_BLANKING_REGISTER, MT9V034_CONTEXT_B_VER_BLANKING_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_1_REGISTER, MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_1_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_2_REGISTER, MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_2_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_CTRL_REGISTER, MT9V034_CONTEXT_B_COARSE_SHUTTER_WIDTH_CTRL_REGISTER_DEFAULT_VALUE);
  res = res && this->write_register(MT9V034_CONTEXT_B_READ_MODE_REGISTER, MT9V034_CONTEXT_B_READ_MODE_REGISTER_DEFAULT_VALUE);

  return res;
}

uint8_t MT9V034::select_mode(MT9V034_mode mode) {
  if (this->_mode != mode || !this->_initialized) {
    this->_mode = mode;
    this->dcmi_dma_disable();
    this->dcmi_and_dma_init();
    this->dcmi_dma_enable();
    this->read_register(0x6B, (uint8_t *)&this->operation_mode);
    if (mode == FLOW_MODE) {
      // switch to context A
      return this->write_register(MT9V034_CHIP_CONTROL_REGISTER, MT9V034_CHIP_CONTROL_REGISTER_DEFAULT_VALUE);
    } else {
      // switch to context B
      return this->write_register(MT9V034_CHIP_CONTROL_REGISTER, MT9V034_CHIP_CONTROL_REGISTER_DEFAULT_VALUE | 0x8000);
    }
  }
  
  return 1;
}

uint8_t MT9V034::dcmi_pin_init() {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  /* Connect DCMI pins to AF13 */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_DCMI); //DCMI_HSYNC
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_DCMI); //DCMI_PIXCL

	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_DCMI); //DCMI_D5
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_DCMI); //DCMI_VSYNC

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_DCMI); //DCMI_D0
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_DCMI); //DCMI_D1

	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_DCMI); //DCMI_D8
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_DCMI); //DCMI_D9

	GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF_DCMI); //DCMI_D2
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource1, GPIO_AF_DCMI); //DCMI_D3
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource4, GPIO_AF_DCMI); //DCMI_D4
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_DCMI); //DCMI_D6
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource6, GPIO_AF_DCMI); //DCMI_D7

	/* DCMI GPIO configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_12;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4
			| GPIO_Pin_5 | GPIO_Pin_6;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

  return 1;
}

uint8_t MT9V034::dcmi_and_dma_init() {
  DCMI_InitTypeDef DCMI_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	/*** Configures the DCMI to interface with the mt9v034 camera module ***/
	/* Enable DCMI clock */
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);

	/* DCMI configuration */
  if (this->_mode == FLOW_MODE) {
	  DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_SnapShot;
  } else {
    DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_SnapShot;
  }
	DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
	DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Falling;
	DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_Low;
	DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
	DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
	DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_10b;

  DCMI_Init(&DCMI_InitStructure);

	/* Configures the DMA2 to transfer Data from DCMI */
	/* Enable DMA2 clock */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	/* DMA2 Stream1 Configuration */
	DMA_DeInit(DMA2_Stream1);

	DMA_InitStructure.DMA_Channel = DMA_Channel_1;
	DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) this->_dma_buffer_0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = MT9V034_OF_MODE_DMA_BUFFER_SIZE / 4; // buffer size in date unit (word)
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream1, &DMA_InitStructure);
  
  NVIC_InitTypeDef NVIC_InitStructure;

  /* 
   * Enable DCMI Interrupt 
   * DCMI interrupt needs to be lower than DMA interrupt
  */
	NVIC_InitStructure.NVIC_IRQChannel = DCMI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

  NVIC_InitTypeDef NVIC_InitStructure1;
	NVIC_InitStructure1.NVIC_IRQChannel = DMA2_Stream1_IRQn;
	NVIC_InitStructure1.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure1.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure1.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure1);

  return 1;
}

uint8_t MT9V034::start_image_capture() {
  DCMI_CaptureCmd(ENABLE);
  return 1;
}

uint8_t MT9V034::stop_image_capture() {
  DCMI_CaptureCmd(DISABLE);
  return 1;
}

uint8_t MT9V034::dcmi_dma_enable() {
  DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);
  DCMI_Cmd(ENABLE);
  DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
  DMA_Cmd(DMA2_Stream1, ENABLE);
  return 1;
}

uint8_t MT9V034::dcmi_dma_disable() {
  DMA_Cmd(DMA2_Stream1, DISABLE);
  DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);
  DCMI_ITConfig(DCMI_IT_FRAME, DISABLE);
	DCMI_Cmd(DISABLE);
  return 1;
}

uint8_t MT9V034::write_register(uint8_t address, uint16_t value) {
  uint8_t res = 0;

  /** because stm32 is little edian, so before sending the byte
      we have to put the least significant byte at index 0. **/
  uint8_t bytes_to_be_sent[2];
  bytes_to_be_sent[0] = value >> 8;
  bytes_to_be_sent[1] = value & 0x00FF;
  res = this->_i2c_host->write(MT9V034_I2C_DEVICE_ADDRESS, address, bytes_to_be_sent, 2);

  /**
   * After writing the value, reading the value again is essential to make sure data is written as expected.
   * However, because of the existence of shadow registers, the register value won't change immediately.
   */
  // uint8_t bytes_received[2];
  // res = res && this->read_register(address, bytes_received);
  
  // res = res 
  //       && (bytes_received[0] == (value >> 8))
  //       && (bytes_received[1] == (value & 0x00FF));

  return res;
}

uint8_t MT9V034::read_register(uint8_t address, uint8_t *value) {
  uint8_t res = 0;
  res = this->_i2c_host->read(MT9V034_I2C_DEVICE_ADDRESS, address, value, 2);
  return res;
}