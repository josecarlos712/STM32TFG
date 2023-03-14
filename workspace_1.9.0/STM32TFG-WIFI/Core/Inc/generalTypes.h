/*
 * generalTypes.h
 *
 *  Created on: Mar 29, 2022
 *      Author: josec
 */

#ifndef INC_GENERALTYPES_H_
#define INC_GENERALTYPES_H_

#pragma GCC diagnostic ignored "-Wuninitialized"
//Includes
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
//Definicion de constantes relativas a las instrucciones
static const uint8_t I_NUM_INSTRUCTIONS = 5; //Numero total de instrucciones del repertorio de movimientos
static const uint8_t I_MAX_TIME_EXECUTION = 100; //Tiempo maximo (en decimas) que puede ejecutarse una instruccion
//Definiciones de codigos para cada movimiento
static const uint8_t I_NOPE = 0; //NP
static const uint8_t I_FORDWARD = 1; //FWD
static const uint8_t I_BACKWARD = 2; //BWD
static const uint8_t I_ROTATE_CLOCKWISE = 3; //RCW
static const uint8_t I_ROTATE_COUNTERCLOCKWISE = 4; //RCC
static const uint8_t I_BREAK = 5; //BRK
//Definiciones de constantes
static const uint8_t NUM_CHARS_PER_INSTRUCTION = 7;
//Definicion de estructuras
typedef struct {
	uint8_t instruction;
	uint8_t duration;
	//toString format: [COMMAND(3)][TIME_DECS(4)]
} MovementInstruction_t;

//Definicion de funciones
/*
 * Transforma un struct de instruccion de movimiento en una cadena y lo devuelve mediante un buffer. Dado:
 * - MovementInstruction_t* pMovIns - Puntero al struct de instruccion de movimiento
 * - char** buf - Buffer de salida por el que se devuelve la cadena.
 */
void I_MovementInstruction_toString(MovementInstruction_t* pMovIns, char** buf);

/*
 * Crea un struct de instruccion de movimiento dado:
 * - int instCode - Codigo de la instruccion de movimiento
 * - int time - Tiempo de ejecucion de la instruccion
 * - MovementInstruction_t* pMov - Puntero del buffer de salida
 * La función revisa si el codigo de instruccion de movimiento existe. Si no, devuelve una instruccion NOPE de duracion /time/
 */
uint8_t I_CreateInstructionStruct(uint8_t instCode, uint8_t time, MovementInstruction_t* pMov);

#endif /* INC_GENERALTYPES_H_ */