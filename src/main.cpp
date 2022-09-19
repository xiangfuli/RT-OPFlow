#include "main.hpp"

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

void *operator new(size_t size) {
    return pvPortMalloc(size);
}

void operator delete(void *ptr) {
    vPortFree(ptr);
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
        vTaskDelay(pdMS_TO_TICKS(500));
	}
}

int main(int ac, char** av) {
		SystemInit();
    xTaskCreate(pvFCSTaskPowerLEDBlink, "pvFCSTaskPowerLEDBlink", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, NULL);

		/* init usb */
		USBD_Init(&USB_OTG_dev,
					USB_OTG_FS_CORE_ID,
					&USR_desc,
					&USBD_CDC_cb,
					&USR_cb);

    vTaskStartScheduler();

    SensorsManager sensors_manager;
    sensors_manager.camera_connected = 1;

    while(1) {}
}