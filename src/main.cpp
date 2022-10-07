#include "main.hpp"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

void *operator new(size_t size) {
  return pvPortMalloc(size);
}

void *operator new[](size_t size){
	return pvPortMalloc(size); 
}

void operator delete(void *ptr) {
  vPortFree(ptr);
}

void operator delete[](void *p){
	vPortFree(p); 
}

uint8_t number_of_devices = 1;
uint8_t *i2c_device_address;
uint8_t previous_image[(MT9V034_CAMERA_FULL_HEIGHT / 4) * (MT9V034_CAMERA_FULL_WIDTH / 4)] 	__attribute__((section(".ccmram")));
uint8_t latest_image[(MT9V034_CAMERA_FULL_HEIGHT / 4) * (MT9V034_CAMERA_FULL_WIDTH / 4)] 	 	__attribute__((section(".ccmram")));

OpFlowBoardAlpha board;
I2CHost i2c_host(&board);
MessageManager message_manager;
StorageManager storage_manager(&i2c_host);
ConfigManager config_manager(&storage_manager);
SystemManager system_manager(&config_manager, &message_manager);
SensorManager sensor_manager(&i2c_host, &message_manager);
SensorMessageAgent sensor_message_agent(&message_manager, &sensor_manager);
// OpticalFlow flow(MT9V034_CAMERA_FULL_HEIGHT / 4, MT9V034_CAMERA_FULL_WIDTH / 4, 17, 20, SSD_BLOCK_MATCH);


uint8_t initialization_res;

void I2C2_ER_IRQHandler() {
	I2CHost_error_handler_callback(&i2c_host);
}

void DMA2_Stream1_IRQHandler(void) {
	sensor_manager_dcmi_dma_it_handler(&sensor_manager);
	return;
}

void DCMI_IRQHandler(void) {
	sensor_manager_dcmi_it_handler(&sensor_manager);
	return;
}

void message_manager_receive_usb_data_wrapper(uint8_t *buffer, uint32_t len) {
  message_manager.receive_message_from_usb(buffer, len);
}

void pvFCSTaskPowerLEDBlink(void *pvParameters) {
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
		if (initialization_res) {
			vTaskDelay(pdMS_TO_TICKS(200));
		} else {
			vTaskDelay(pdMS_TO_TICKS(500));
		}
	}
}

int main(void) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	initialization_res = i2c_host.init(); 
	initialization_res = initialization_res && message_manager.init();
	initialization_res = initialization_res && config_manager.init();
	initialization_res = initialization_res && system_manager.init();
	initialization_res = initialization_res && sensor_manager.init();
	
	message_manager.start_transmission_task();
	storage_manager.task_start();
	system_manager.tasks_start();

	sensor_manager.select_camera_operation_mode(HR_MODE);

	/* init usb */
	USBD_Init(&USB_OTG_dev,
		USB_OTG_FS_CORE_ID,
		&USR_desc,
		&USBD_CDC_cb,
		&USR_cb);
	
	previous_image[0] = 1;
	latest_image[0] = 1;

	xTaskCreate(pvFCSTaskPowerLEDBlink, "pvFCSTaskPowerLEDBlink", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, NULL);
	vTaskStartScheduler();
	while(1) {}
}