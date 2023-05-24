/*
 * HAL_Instructions.c
 *
 *  Created on: 5 abr. 2022
 *      Author: josec
 *
 *      Aquí van todas las funciones que hagan referencia al bajo nivel:
 *      - Enviar por el puerto serie
 *      - Recibir por el puerto serie
 *      - Establecer un temporizador para una instruccion
 */
#include "HAL_Instructions.h"

void S_SendInstruction(uint8_t instCode, uint8_t time) {
	//Creo el struct de instruccion
	MovementInstruction_t *pMov = malloc(sizeof(MovementInstruction_t));
	I_CreateInstructionStruct(instCode, time, pMov);
	//Transformo la instruccion a una cadena
	char *s_mov;
	I_MovementInstruction_toString(pMov, &s_mov);
	//Reservo el puerto serie
	while (xSemaphoreTake(xSemaphoreSerialHandle, portMAX_DELAY) != pdTRUE)
		;
	//Envio el struct por el puerto serie
	printf("%s\r\n", s_mov);
	//Devuelvo el control del puerto serie
	xSemaphoreGive(xSemaphoreSerialHandle);
	//Libero la memoria reservada por malloc
	free(pMov);
}

void S_SendInstructionStruct(MovementInstruction_t *pMov) {
	//Transformo la instruccion a una cadena
	char *s_mov;
	I_MovementInstruction_toString(pMov, &s_mov);
	//Reservo el puerto serie
	while (xSemaphoreTake(xSemaphoreSerialHandle, portMAX_DELAY) != pdTRUE)
		;
	//Envio el struct por el puerto serie
	printf("%s\r\n", s_mov);
	//Devuelvo el control del puerto serie
	xSemaphoreGive(xSemaphoreSerialHandle);
}

void S_PrintOnSerial(const char *format, ...) {
	while (xSemaphoreTake(xSemaphoreSerialHandle, portMAX_DELAY) != pdTRUE)
		;
	va_list args;
	va_start(args, format);
	printf(format, args);
	va_end(args);
	// Give the semaphore to indicate that the transmission has finished
	xSemaphoreGiveFromISR(xSemaphoreSerialHandle, pdFALSE);
	// Yield to higher priority task if necessary
	portYIELD_FROM_ISR(pdFALSE);
}
