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
	printf("%s\n", s_mov);
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
	printf("%s\n", s_mov);
	//Devuelvo el control del puerto serie
	xSemaphoreGive(xSemaphoreSerialHandle);
}

void S_SendInstructionToQueue(uint8_t instCode, uint8_t time) {
	//Reservo memoria para el puntero de instruccion
	MovementInstruction_t *pMov = malloc(sizeof(MovementInstruction_t));
	//Creo el struct que se va a enviar a la cola
	I_CreateInstructionStruct(instCode, time, pMov);
	xQueueSend(instructionQueueHandle, pMov, 1);
	//vTaskDelay(100);
	//Libero la memoria del puntero de movimiento
	free(pMov);
}

void Mov_MoveRelativeDirection(int x, int y) {
	// Se hace una conversion de centrimetros a segundos (decimas de segundo) (el factor aún está por determinar mediante una calibración)
	uint8_t tx = ((float) x * MOV_SCALE_MOVEMENT_FACTOR);
	uint8_t ty = ((float) y * MOV_SCALE_MOVEMENT_FACTOR);
	// Se envian los movimientos a la cola
	// Movimiento frontal
	if (y > 0) {
		S_SendInstructionToQueue(I_FORDWARD, ty);
	} else if (y < 0) {
		S_SendInstructionToQueue(I_BACKWARD, ty);
	}
	// Giro
	// Se decide hacia donde será el giro. Se intentará siempre ir hacia adelante pero se prioriza que el tiempo de giro sea el menor posible
	if (x > 0) {
		S_SendInstructionToQueue(I_ROTATE_CLOCKWISE, I_90DEGREES_ROTATION_TIME);
	} else if (x < 0) {
		S_SendInstructionToQueue(I_ROTATE_COUNTERCLOCKWISE, I_90DEGREES_ROTATION_TIME);
	}
	// Movimiento lateral
	S_SendInstructionToQueue(I_FORDWARD, tx);
}
