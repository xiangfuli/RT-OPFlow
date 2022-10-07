#include "main.hpp"

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

OpFlowBoardAlpha board;
SystemManager system_manager(&board);

void I2C2_ER_IRQHandler() {
	I2CHost_error_handler_callback(system_manager.get_i2c_host());
}

void DMA2_Stream1_IRQHandler(void) {
	sensor_manager_dcmi_dma_it_handler(system_manager.get_sensor_manager());
	return;
}

void DCMI_IRQHandler(void) {
	sensor_manager_dcmi_it_handler(system_manager.get_sensor_manager());
	return;
}

void message_manager_receive_usb_data_wrapper(uint8_t *buffer, uint32_t len) {
  system_manager.get_message_manager()->receive_message_from_usb(buffer, len);
}

int main(void) {
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	system_manager.init();
	
	vTaskStartScheduler();
	while(1) {}
}