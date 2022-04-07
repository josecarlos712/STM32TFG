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

/*
 * Envia una instruccion por el puerto serie dado:
 * 	int instCode - Codigo numerico de la instruccion (generalTypes.h)
 * 	int time - Tiempo en decimas de segundo
 *
 * 	Hay que liberar la memoria reservada por malloc en la funcion I_CreateInstructionStruct.
 */
void S_SendInstruction(uint8_t instCode, uint8_t time);

/*
 *
 */



#endif /* INC_HAL_INSTRUCTIONS_H_ */
