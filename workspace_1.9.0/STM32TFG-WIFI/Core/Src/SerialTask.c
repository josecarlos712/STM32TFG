#include "SerialTask.h"

PUTCHAR_PROTOTYPE {
	// Transmits character ch over UART1 and UART4
	while (HAL_OK != HAL_UART_Transmit_IT(&huart1, (uint8_t*) &ch, 1)) {
		// Wait for transmit to finish
		;
	}
	while (HAL_OK != HAL_UART_Transmit_IT(&huart4, (uint8_t*) &ch, 1)) {
		// Wait for transmit to finish
		;
	}
	return ch;
}

int printTransmision = 0;

void serialRxTask(void *parg) {
	uint8_t ins = 0;
	uint8_t time = 20;
	uint8_t i = 0;

	printf("rxtask");

	while (i < 25) {
		// Allocate memory for a MovementInstruction_t struct
		MovementInstruction_t *pMov = malloc(sizeof(MovementInstruction_t));
		// Create the struct to be sent to the queue
		I_CreateInstructionStruct(ins, time, pMov);
		// Send the struct to the queue
		xQueueSend(instructionQueueHandle, pMov, 1);
		// Delay for 1 second
		vTaskDelay(1000);
		// Free the memory of the MovementInstruction_t struct
		free(pMov);
		// Update the instruction to be sent
		ins = (ins + 1) % I_NUM_INSTRUCTIONS;
		i++;
	}
}

void serialTxTask(void *parg) {
	while (1) {
		MovementInstruction_t pMov;
		// Receive the instruction from the queue
		xQueueReceive(instructionQueueHandle, &pMov, portMAX_DELAY);
		// Send the instruction over the serial port
		S_SendInstructionStruct(&pMov);
		// Wait for the duration of the instruction
		vTaskDelay(50);
	}
}

void WebServerTask(void *pArg) {
	wifi_server();
}

void CreateSerialObjects() {
	// Create a queue with 16 elements, each element has a size of 1 byte
	instructionQueueHandle = xQueueCreate(16, 1);
}

void CreateSerialTask() {
	xTaskCreate(WebServerTask, "TareaWebServer", 256, NULL, 2, NULL);
	xTaskCreate(serialTxTask, "serialTxTask", 256, NULL, 1, NULL);
	xTaskCreate(serialRxTask, "serialRxTask", 256, NULL, 1, NULL);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	static signed long xHigherPriorityTaskWoken = pdFALSE;

	// Give the semaphore to indicate that the transmission has finished
	xSemaphoreGiveFromISR(xSemaphoreSerialHandle, xHigherPriorityTaskWoken);
	// Yield to higher priority task if necessary
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
