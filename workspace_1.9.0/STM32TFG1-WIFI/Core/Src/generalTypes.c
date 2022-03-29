/*
 * generalTypes.c
 *
 *  Created on: Mar 29, 2022
 *      Author: josec
 */

#include "generalTypes.h"

void MovementInstruction_toString(MovementInstruction_t* pMovIns, char** buf) {

	char* s = (char*)malloc(NUM_CHARS_PER_INSTRUCTION);

	sprintf(s, "%d %d\0", pMovIns->instruction, pMovIns->duration);

	*buf = s;
}
