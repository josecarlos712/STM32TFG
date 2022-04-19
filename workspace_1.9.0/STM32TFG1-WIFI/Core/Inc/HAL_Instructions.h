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

static const float MOV_SCALE_MOVEMENT_FACTOR = 2.0f;

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

/*
 * Divide una direccion vectorial de la forma (x,y) en instrucciones de movimiento y las añade a la cola para que se envien por el puerto serie.
 *  double x - Movimiento en el eje X
 *  double y - Movimiento en el eje Y
 * Las unidades de x e y van en centimetros relativos a la posicion actual del vehiculo por lo que han de multiplicarse por un factor de conversion de centrimetros a segundos de movimiento.
 * El sistema de referencia es relativo al vehiculo. Siendo el eje Y positivo hacia adelante y el eje X positivo hacia la derecha.
 */
void Mov_MoveRelativeDirection(int x, int y);

/*
 * TO-DO. Funcion de movimiento Mov_MoveDirection con un sistema de referencia absoluto en vez de relativo, estableciendo este sistema con al iniciar el sistema.
 */

#endif /* INC_HAL_INSTRUCTIONS_H_ */
