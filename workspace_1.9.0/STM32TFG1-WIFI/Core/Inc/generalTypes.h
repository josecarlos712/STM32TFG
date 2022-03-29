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
//Definiciones de codigos para cada movimiento
static const uint8_t NOPE = 0; //NP
static const uint8_t FORDWARD = 1; //FWD
static const uint8_t BACKWARD = 2; //BWD
static const uint8_t ROTATE_CLOCKWISE = 3; //RCW
static const uint8_t ROTATE_COUNTERCLOCKWISE = 4; //RCC
static const uint8_t BREAK = 5; //BRK
//Definiciones de constantes
static const uint8_t NUM_CHARS_PER_INSTRUCTION = 7;
//Definicion de estructuras
typedef struct {
	uint8_t instruction;
	uint8_t duration;
	//toString format: [COMMAND(3)][TIME_DECS(4)]
} MovementInstruction_t;

//Definicion de funciones
void MovementInstruction_toString(MovementInstruction_t* pMovIns, char** buf); //Funcion toString del struct MovementInstruction

#endif /* INC_GENERALTYPES_H_ */
