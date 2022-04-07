/*
 * generalTypes.c
 *
 *  Created on: Mar 29, 2022
 *      Author: josec
 */

#include "generalTypes.h"

void I_MovementInstruction_toString(MovementInstruction_t *pMovIns, char **buf) {

	char *s = (char*) malloc(NUM_CHARS_PER_INSTRUCTION);

	sprintf(s, "%d %d", pMovIns->instruction, pMovIns->duration);

	*buf = s;
}

uint8_t I_CreateInstructionStruct(uint8_t instCode, uint8_t time,
		MovementInstruction_t *pMovOut) {
	// Comprueba que todos los parametros sean correctos
	if (instCode < NUM_INSTRUCTIONS) {
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
