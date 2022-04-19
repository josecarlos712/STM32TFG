/*
 * MathFunctions.c
 *
 *  Created on: 19 abr. 2022
 *      Author: josec
 */
#include "MathFunctions.h"

double Math_abs(double x) {

	if (x < 0) {
		return x * -1;
	}
	return x;
}
