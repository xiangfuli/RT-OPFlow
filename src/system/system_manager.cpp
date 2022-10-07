#include "system_manager.hpp"

uint8_t previous_image[(MT9V034_CAMERA_FULL_HEIGHT / 4) * (MT9V034_CAMERA_FULL_WIDTH / 4)] 	__attribute__((section(".ccmram")));
uint8_t latest_image[(MT9V034_CAMERA_FULL_HEIGHT / 4) * (MT9V034_CAMERA_FULL_WIDTH / 4)] 	 	__attribute__((section(".ccmram")));

void system_task_lifecounting_wrapper(void *pvParameters) {
  static_cast<SystemManager *>(pvParameters)->task_lifetime_counting();
}

void system_task_update_led_according_to_system_state(void *pvParameters) {
  static_cast<SystemManager *>(pvParameters)->task_update_led_according_to_system_state();
}

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

SystemManager::SystemManager(OpFlowBoard *board) {
  this->_i2c_host = new I2CHost(board);
  this->_message_manager = new MessageManager();
  this->_storage_manager = new StorageManager(this->_i2c_host);
  this->_config_manager = new ConfigManager(this->_storage_manager);
  this->_sensor_manager = new SensorManager(this->_i2c_host, this->_message_manager);
  this->_sensor_message_agent = new SensorMessageAgent(this->_message_manager, this->_sensor_manager);
  this->_system_message_agent = new SystemMessageAgent(this->_message_manager);

  this->_optiocal_flow = new OpticalFlow(MT9V034_CAMERA_FULL_HEIGHT/4, MT9V034_CAMERA_FULL_WIDTH/4, 17, 20, SSD_BLOCK_MATCH);
}

SystemManager::~SystemManager() {}

uint8_t SystemManager::init() {
  uint8_t init_result = 1;
  init_result = init_result && this->_i2c_host->init();
  init_result = init_result && this->_message_manager->init();
  init_result = init_result && this->_config_manager->init();
  init_result = init_result && this->_sensor_manager->init();

  if (init_result != 1) {
    this->_system_state = INITIALIZATION_FAILED;
  } else {
    this->_system_state = RUNNING;
  }
  
  if (this->_system_state == RUNNING) {
    this->load_launch_times();
    this->load_system_lifetime();
    this->increase_launch_times();

    this->_message_manager->start_transmission_task();
    this->_storage_manager->task_start();
    
    this->_sensor_manager->select_camera_operation_mode(HR_MODE);
  }

  /* init usb */
	USBD_Init(&USB_OTG_dev,
		USB_OTG_FS_CORE_ID,
		&USR_desc,
		&USBD_CDC_cb,
		&USR_cb);

  this->tasks_start();
  return init_result;
}

void SystemManager::load_system_lifetime() {
  // Load needed configurations
  this->_lifetime_byte_0 = this->_config_manager->_system_lifetime_byte_0->get_value();
  this->_lifetime_byte_1 = this->_config_manager->_system_lifetime_byte_1->get_value();
  this->_lifetime_byte_2 = this->_config_manager->_system_lifetime_byte_2->get_value();
  this->_lifetime_byte_3 = this->_config_manager->_system_lifetime_byte_3->get_value();
}

void SystemManager::load_launch_times() {
  this->_launch_time_byte_0 = this->_config_manager->_system_launch_times_byte_0->get_value();
  this->_launch_time_byte_1 = this->_config_manager->_system_launch_times_byte_1->get_value();
}

void SystemManager::increase_launch_times() {
  uint16_t times = (((uint16_t)this->_launch_time_byte_0) << 8) | ((uint16_t)this->_launch_time_byte_1);
  times++;
  this->_launch_time_byte_0 = times >> 8;
  this->_launch_time_byte_1 = times & 0x00FF;
  this->_config_manager->store_cofig(this->_config_manager->_system_launch_times_byte_0, this->_launch_time_byte_0);
  this->_config_manager->store_cofig(this->_config_manager->_system_launch_times_byte_1, this->_launch_time_byte_1);
}

void SystemManager::tasks_start() {
  xTaskCreate(
    system_task_update_led_according_to_system_state,
    "system_task_update_led_according_to_system_state",
    configMINIMAL_STACK_SIZE,
    this, 
    configMAX_PRIORITIES, 
    NULL
  );
  if (this->_system_state == RUNNING) {
    xTaskCreate(
    system_task_lifecounting_wrapper,
    "system_lifetime_counting",
    configMINIMAL_STACK_SIZE,
    this, 
    configMAX_PRIORITIES, 
    NULL
  );
  }
}

void SystemManager::task_update_led_according_to_system_state() {
  GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	uint8_t bit = 0;
	while(1) {
		GPIO_WriteBit(GPIOD, GPIO_Pin_11, BitAction(bit));
		bit = !bit;
		if (this->_system_state == RUNNING) {
			vTaskDelay(pdMS_TO_TICKS(1000));
		} else {
			vTaskDelay(pdMS_TO_TICKS(100));
		}
	}
}

void SystemManager::task_lifetime_counting() {
  TickType_t xLastWakeTime;
	const TickType_t xFrequency = configTICK_RATE_HZ;
	while(1) {
		xLastWakeTime = xTaskGetTickCount();
		this->_lifetime_byte_3 += 1;
		this->_config_manager->store_cofig(
      this->_config_manager->_system_lifetime_byte_3,
      this->_lifetime_byte_3
    );
		if (this->_lifetime_byte_3 == 0) {
			this->_lifetime_byte_2 += 1;
			this->_config_manager->store_cofig(
        this->_config_manager->_system_lifetime_byte_2,
        this->_lifetime_byte_2
      );
			if (this->_lifetime_byte_2 == 0) {
				this->_lifetime_byte_1 += 1;
				this->_config_manager->store_cofig(
          this->_config_manager->_system_lifetime_byte_1,
          this->_lifetime_byte_1
        );
				if (this->_lifetime_byte_1 == 0) {
					this->_lifetime_byte_1 += 1;
					this->_config_manager->store_cofig(
            this->_config_manager->_system_lifetime_byte_0,
            this->_lifetime_byte_0
          );
					if (this->_lifetime_byte_0 == 0) {
						// That's impossible
					}
				}
			}
		}
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
	}
}

I2CHost *SystemManager::get_i2c_host() {
  return this->_i2c_host;
}

SensorManager *SystemManager::get_sensor_manager() {
  return this->_sensor_manager;
}

MessageManager *SystemManager::get_message_manager() {
  return this->_message_manager;
}