#include "SerialTask.h"

PUTCHAR_PROTOTYPE {

	while (HAL_OK != HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 30000)) {
		;
	}
	//xSemaphoreTake(xSemaphore, portMAX_DELAY);
	//HAL_UART_Transmit_IT(&huart1, (uint8_t*) &ch, 1);
	return ch;

}

GETCHAR_PROTOTYPE {

	uint8_t ch = 0;
	while (HAL_OK != HAL_UART_Receive(&huart1, (uint8_t*) &ch, 1, 30000)) {
		;
	}
	return ch;
}

void serialRxTask(void *parg) {

	//char c;

	while (1) {
		//xQueueReceive(xQueue, &c, portMAX_DELAY);
		//printf("Dato recibido: %c", c);
		//vTaskDelay(200);
	}

}

/*void sendQeue() {
 while(1) {
 xQueueSend(xQueue, "H", 1);
 vTaskDelay(200);
 }
 }*/

void serialTxTask(void *parg) {
	while (1) {
		uint8_t ins = I_FORDWARD;
		uint8_t time = 10;
		S_SendInstruction(ins, time);
		vTaskDelay(time*100);
	}
	/*
	 * TO-DO
	 *
	 * - Probar que S_SendInstruction imprime correctamente.
	 * - Añadir semaforo que regule el uso compartido del puerto serie.
	 */

}

void CreateSerialObjects() {
	//Liberamos el semaforo
	xSemaphoreGive(xSemaphoreSerialHandle);

	//Creamos una cola de 16 elementos en la que cada elemento tiene 1 byte
	//xQueue = xQueueCreate(16, 1);
}

void CreateSerialTask() {

	//xTaskCreate(serialTxTask, "serialTxTask", 256, NULL, 1,
	//NULL);
	//xTaskCreate(serialRxTask, "serialRxTask", 256, NULL, 1,
	//NULL);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	//BaseType_t hasWaken;
	//xSemaphoreGiveFromISR(xSemaphore, &hasWaken);
	//portYIELD_FROM_ISR(hasWaken);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	//char c;
	//c = HAL_UART_Receive_IT(&huart1, (uint8_t*) &c, 1);
	//xQueueSendToBackFromISR(xQueue, &c, NULL);
}

