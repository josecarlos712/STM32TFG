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

/**
 * Envía una instrucción a través del puerto serie.
 *
 * Parámetros:
 * - instCode: código de la instrucción.
 * - time: tiempo de ejecución de la instrucción.
 */
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
	printf("%s\r\n", s_mov);
	//Devuelvo el control del puerto serie
	xSemaphoreGive(xSemaphoreSerialHandle);
	//Libero la memoria reservada por malloc
	free(pMov);
}

/**
 * Envía una estructura de instrucción a través del puerto serie.
 *
 * Parámetros:
 * - pMov: puntero a la estructura de instrucción.
 */
void S_SendInstructionStruct(MovementInstruction_t *pMov) {
	//Transformo la instruccion a una cadena
	char *s_mov;
	I_MovementInstruction_toString(pMov, &s_mov);
	//Reservo el puerto serie
	while (xSemaphoreTake(xSemaphoreSerialHandle, portMAX_DELAY) != pdTRUE)
		;
	//Envio el struct por el puerto serie
	printf("%s\r\n", s_mov);
	//Devuelvo el control del puerto serie
	xSemaphoreGive(xSemaphoreSerialHandle);
}

/**
 * Imprime en el puerto serie utilizando formato.
 *
 * Parámetros:
 * - format: cadena de formato.
 * - ...: argumentos variables para el formato.
 */
void S_PrintOnSerial(const char *format, ...) {
	while (xSemaphoreTake(xSemaphoreSerialHandle, portMAX_DELAY) != pdTRUE)
		;
	va_list args;
	va_start(args, format);
	printf(format, args);
	va_end(args);
	// Give the semaphore to indicate that the transmission has finished
	xSemaphoreGiveFromISR(xSemaphoreSerialHandle, pdFALSE);
	// Yield to higher priority task if necessary
	portYIELD_FROM_ISR(pdFALSE);
}

/**
 * Convierte un objeto JSON a instrucciones en formato uint8_t.
 *
 * Parámetros:
 * - json: cadena que representa el objeto JSON.
 * - iJson: array de salida para los valores de "ins".
 * - iSize: puntero al tamaño del array "ins".
 * - dJson: array de salida para los valores de "dur".
 * - dSize: puntero al tamaño del array "dur".
 *
 * Retorna:
 * - 0 si la conversión fue exitosa, o un código de error si ocurrió algún problema.
 */
uint8_t I_JsonToInstructions(const char *json, uint8_t *iJson, uint8_t *iSize,
		uint8_t *dJson, uint8_t *dSize) {

	// Obtener los valores de "ins" y "dur" del JSON
	const char *insJson = findJsonValue(json, "\"ins\"");
	const char *durJson = findJsonValue(json, "\"dur\"");

	// Contar el número de elementos en cada array JSON
	uint8_t insSize = 1; // Inicializar en 1, ya que siempre hay al menos un elemento
	uint8_t durSize = 1;

	// Contar los elementos en el array "ins"
	const char *ptr = insJson;
	while (*ptr != ']') {
		if (*ptr == ',') {
			++insSize;
		}
		++ptr;
	}

	// Contar los elementos en el array "dur"
	ptr = durJson;
	while (*ptr != ']') {
		if (*ptr == ',') {
			++durSize;
		}
		++ptr;
	}

	uint8_t instructionJson[insSize];
	uint8_t durationJson[durSize];

	// Convertir los elementos del array "ins" y "dur" a uint8_t
	parseJsonArray(insJson, instructionJson, insSize);
	parseJsonArray(durJson, durationJson, durSize);

	// Copiar los arrays resultantes a los arrays de salida
	memcpy(iJson, (uint8_t*) instructionJson, insSize);
	memcpy(dJson, (uint8_t*) durationJson, durSize);

	// Asignar los tamaños de los arrays de salida
	(*iSize) = insSize;
	(*dSize) = durSize;

	return 0;
}

// Encuentra el valor de una clave en una cadena JSON
// Parámetros:
// - json: cadena JSON en la que se buscará la clave y su valor
// - key: clave que se desea encontrar en el JSON
// Retorna:
// - Puntero al inicio del valor de la clave encontrada, o NULL si no se encuentra la clave
const char* findJsonValue(const char *json, const char *key) {
    const char *start = strstr(json, key);
    if (start == NULL) {
        return NULL;
    }
    start += strlen(key) + 2; // se salta la clave y ": ["

    return start;
}


// Analiza un array en formato JSON y guarda los valores en un array de enteros
// Parámetros:
// - jsonArray: cadena que representa el array en formato JSON
// - array: array de enteros en el que se guardarán los valores
// - size: tamaño del array de enteros y del array JSON
void parseJsonArray(const char *jsonArray, uint8_t *array, size_t size) {
    const char *ptr = jsonArray;

    ptr++; // Para omitir el primer '['
    for (size_t i = 0; i < size; i++) {
        array[i] = (uint8_t) atoi(ptr);
        ptr = strchr(ptr, ',') + 2; // se salta el valor actual y ", "
    }
}


