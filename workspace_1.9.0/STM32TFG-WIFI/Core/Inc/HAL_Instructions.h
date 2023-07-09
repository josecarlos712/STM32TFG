/*
 * HAL_Instructions.h
 *
 *  Created on: 5 abr. 2022
 *      Author: josec
 *
 *      Aquí van todas las funciones que hagan referencia al bajo nivel:
 *      - Enviar por el puerto serie
 *      - Recibir por el puerto serie
 *      - Establecer un temporizador para una instruccion
 */

#ifndef INC_HAL_INSTRUCTIONS_H_
#define INC_HAL_INSTRUCTIONS_H_

#include "generalTypes.h"
#include "SerialTask.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_NUM_INSTRUCTIONS 100

/*
 * Envia una instruccion por el puerto serie dado:
 * 	int instCode - Codigo numerico de la instruccion (generalTypes.h)
 * 	int time - Tiempo en decimas de segundo
 *
 * 	Hay que liberar la memoria reservada por malloc en la funcion I_CreateInstructionStruct.
 */
void S_SendInstruction(uint8_t instCode, uint8_t time);

/*
 * Envia una intruccion por el puerto serie dado:
 * 	MovementInstruction_t *pMov - Struct de instruccion de movimiento
 */
void S_SendInstructionStruct(MovementInstruction_t *pMov);

void S_PrintOnSerial(const char*__restrict, ...);

uint8_t I_JsonToInstructions(const char *json, uint8_t *iJson, uint8_t *iSize,
		uint8_t *dJson, uint8_t *dSize);
const char* findJsonValue(const char *json, const char *key);
void parseJsonArray(const char *jsonArray, uint8_t *array, size_t size);

#endif /* INC_HAL_INSTRUCTIONS_H_ */
