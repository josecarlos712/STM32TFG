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
		MovementInstruction_t *pMov;
		xQueueSend(xQueue, pMov, 1);
		vTaskDelay(500);
	}

}

/*void sendQeue() {
 while(1) {
 xQueueSend(xQueue, "H", 1);
 vTaskDelay(200);
 }
 }*/

void serialTxTask(void *parg) {
	uint8_t ins = I_FORDWARD;
	uint8_t time = 10;

	while (1) {
		//Reservo memoria para el puntero de instruccion
		MovementInstruction_t *pMov = malloc(sizeof(MovementInstruction_t));
		//Recibo la instruccion desde la cola
		xQueueReceive(osMessageQueueId_t, pMov, portMAX_DELAY);
		//Envio la instruccion por el puerto serie
		S_SendInstruction(pMov->instruction, pMov->time);
		//Espero durante lo que tarda la instruccion
		vTaskDelay(pMov->time * 100);
		//Libero la memoria del puntero de movimiento
		free(pMov);
	}
	/*
	 * TO-DO
	 *
	 * - Crear cola que saque uno a uno las instrucciones y las imprima
	 * - Crear funciones de alto nivel que engloben varias instrucciones de movimiento
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

