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
	printf("%s\n\r", s_mov);
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
	printf("%s\n\r", s_mov);
	//Devuelvo el control del puerto serie
	xSemaphoreGive(xSemaphoreSerialHandle);
}

void Mov_MoveRelativeDirection(int x, int y) {
	// Se hace una conversion de centrimetros a segundos (decimas de segundo) (el factor aún está por determinar mediante una calibración)
	float tx = x * MOV_SCALE_MOVEMENT_FACTOR;
	float ty = y * MOV_SCALE_MOVEMENT_FACTOR;
	// Se decide hacia donde será el giro. Se intentará siempre ir hacia adelante pero se prioriza que el tiempo de giro sea el menor posible
	uint8_t giro;
	if(x != 0) {
		if (x > 0) {
			giro = I_ROTATE_CLOCKWISE;
		} else {
			giro = I_ROTATE_COUNTERCLOCKWISE;
		}
	}
}
