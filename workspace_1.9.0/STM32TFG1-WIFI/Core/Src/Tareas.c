/*
 * Tareas.c
 *
 *  Created on: 29 nov. 2020
 *      Author: rpaz
 */

#include "main.h"
#include "cmsis_os.h"
#include "SerialTask.h"
#include "FreeRTOS.h"
#include "semphr.h"

extern UART_HandleTypeDef huart1;
SemaphoreHandle_t xSemaphore;
xTaskHandle serialRxTaskHandle;
xTaskHandle serialTxTaskHandle;

int wifi_server(void);

void TareaWebServer(void *pArg) {
	printf("HOLA MUNDO");
	wifi_server();
}