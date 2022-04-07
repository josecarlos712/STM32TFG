/*
 * SerialTask.h
 *
 *  Created on: Nov 26, 2020
 *      Author: Sergio
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32l4xx_hal.h"
#include "main.h"

#include "generalTypes.h"

void CreateSerialTask();
void CreateSerialObjects(void);
void serialRxTask(void * parg);
void serialTxTask(void * parg);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#ifdef __GNUC__

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar(void)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif
