/*
 * generalTypes.c
 *
 *  Created on: Mar 29, 2022
 *      Author: josec
 */

#include "generalTypes.h"


// Convierte una estructura de instrucción de movimiento en una cadena de texto
// Parámetros:
// - pMovIns: puntero a la estructura de instrucción de movimiento
// - buf: puntero a un puntero de caracteres donde se almacenará la cadena resultante
void I_MovementInstruction_toString(MovementInstruction_t *pMovIns, char **buf) {

	char s[NUM_CHARS_PER_INSTRUCTION];

	sprintf(s, "%d %d", pMovIns->instruction, pMovIns->duration);

	*buf = s;
}

// Crea una estructura de instrucción de movimiento con los parámetros proporcionados
// Parámetros:
// - instCode: código de instrucción
// - time: tiempo de ejecución
// - pMovOut: puntero a la estructura de instrucción de movimiento de salida
// Devuelve:
// - 1 si los parámetros son válidos y se creó la estructura de instrucción correctamente
// - 0 si los parámetros son inválidos y se asigna la instrucción NOPE por defecto a la estructura de salida
uint8_t I_CreateInstructionStruct(uint8_t instCode, uint8_t time,
		MovementInstruction_t *pMovOut) {
	// Comprueba que todos los parametros sean correctos
	if (instCode < I_NUM_INSTRUCTIONS) {
		if (time < I_MAX_TIME_EXECUTION) {
			if (pMovOut != NULL) {
				pMovOut->instruction = instCode;
				pMovOut->duration = time;
			}
		}
		return 1;
	}
	// Si no devuelve una instruccion NOPE
	else {
		pMovOut->instruction = I_NOPE;
		pMovOut->duration = I_MAX_TIME_EXECUTION;
		return 0;
	}
}
