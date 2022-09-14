#include "main.h"

void *operator new(size_t size) {
    return pvPortMalloc(size);
}

void operator delete(void *ptr) {
    vPortFree(ptr);
}

void pvFCSTaskPowerLEDBlink(void *pvParameters) {
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	uint8_t bit = 0;
	while(1) {
		GPIO_WriteBit(GPIOC, GPIO_Pin_8, BitAction(bit));
		bit = !bit;
        vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

int main(int ac, char** av) {
    xTaskCreate(pvFCSTaskPowerLEDBlink, "pvFCSTaskPowerLEDBlink", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES, NULL);
    vTaskStartScheduler();

    SensorsManager sensors_manager;
    sensors_manager.camera_connected = 1;

    while(1) {}
}