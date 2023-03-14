/*
 * SerialTask.h
 *
 *  Created on: Nov 26, 2020
 *      Author: Sergio
 */

#define TERMINAL_USE

#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)

#include "HAL_Instructions.h"
#include "FreeRTOS.h"
#include "stm32l4xx_hal.h"
#include "cmsis_os2.h"
#include "semphr.h"

extern UART_HandleTypeDef huart4, huart1;
extern osSemaphoreId_t xSemaphoreSerialHandle;
extern osMessageQueueId_t instructionQueueHandle;
extern osSemaphoreId_t xSemaphoreSerialHandle;

void CreateSerialTask();
void CreateSerialObjects(void);
void serialRxTask(void * parg);
void serialTxTask(void * parg);
void PrintTask(void * parg);
int wifi_server(void);
void TareaWebServer(void *pArg);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif

#ifdef  TERMINAL_USE
#define LOG(a) printf a
#else
#define LOG(a)
#endif
