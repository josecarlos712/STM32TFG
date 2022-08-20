#include "SerialTask.h"

PUTCHAR_PROTOTYPE {

	//xSemaphoreTake(xSemaphoreSerialHandle, portMAX_DELAY);
	while (HAL_OK != HAL_UART_Transmit(&huart4, (uint8_t*) &ch, 1, 30000)) {
		;
	}
	//HAL_UART_Transmit_IT(&huart1, (uint8_t*) &ch, 1);
	return ch;

}

GETCHAR_PROTOTYPE {

	uint8_t ch = 0;
	while (HAL_OK != HAL_UART_Receive(&huart4, (uint8_t*) &ch, 1, 30000)) {
		;
	}
	return ch;
}

void serialRxTask(void *parg) {
	uint8_t ins = 0;
	uint8_t time = 20;
	uint8_t i = 0;

	while (i < 25) {
		//Reservo memoria para el puntero de instruccion
		MovementInstruction_t *pMov = malloc(sizeof(MovementInstruction_t));
		//Creo el struct que se va a enviar a la cola
		I_CreateInstructionStruct(ins, time, pMov);
		xQueueSend(instructionQueueHandle, pMov, 1);
		vTaskDelay(1000);
		//Libero la memoria del puntero de movimiento
		free(pMov);
		// Actualizo la instruccion a enviar
		ins = (ins+1)%I_NUM_INSTRUCTIONS;
		i++;
	}

}

void serialTxTask(void *parg) {

	while (1) {
		MovementInstruction_t pMov;
		//Recibo la instruccion desde la cola
		xQueueReceive(instructionQueueHandle, &pMov, portMAX_DELAY);
		//Envio la instruccion por el puerto serie
		S_SendInstructionStruct(&pMov);
		//Espero durante lo que tarda la instruccion
		//vTaskDelay(pMov.duration * 100);
		vTaskDelay(50);
	}
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
	//xSemaphoreGiveFromISR(xSemaphoreSerialHandle, &hasWaken);
	//portYIELD_FROM_ISR(hasWaken);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	//char c;
	//c = HAL_UART_Receive_IT(&huart1, (uint8_t*) &c, 1);
	//xQueueSendToBackFromISR(xQueue, &c, NULL);
}

