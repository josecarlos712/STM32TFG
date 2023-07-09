#include "HardwareSerial.h"
#include "string.h"
#include "instructions.hpp"
#include "globalConst.hpp"
#include <Arduino.h>

//Variables globales
int puntPaso;
extern int pasos[MAX_NUM_INST];
extern float timepasos[MAX_NUM_INST];

void initTest() {
  int seg = 1;
  //  int temppasos[20] = {1, 1, 2, 2, 3, 4, 1, 5, 1, 1, 2, 2, 3, 4, 1, 5, 0, 0, 0, 0}; //20 elementos
  //  float temptimepasos[20] = {3.0, 2.0, 1.0, 1.0, 3.0, 2.0, 1.0, 5.0, .0, 2.0, 1.0, 1.0, 3.0, 2.0, 1.0, 5.0, 0.0, 0.0, 0.0, 0.0}; //20 elementos
  int temppasos[20] = {1, 1, 2, 2, 3, 4, 1, 5, 1, 1, 2, 2, 3, 4, 1, 5, 0, 0, 0, 0}; //20 elementos
  float temptimepasos[20] = {3.0, 2.0, 1.0, 1.0, 3.0, 2.0, 1.0, 0.0, 0.0, 3.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; //20 elementos
  //float temptimepasos[20] = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; //20 elementos

  memcpy(pasos, temppasos, sizeof(int) * 20);
  memcpy(timepasos, temptimepasos, sizeof(float) * 20);
}

void cleanQueue() {
  for (int i = 0; i < MAX_NUM_INST; i++) {
    pasos[i] = 0;
    timepasos[i] = 0.0;
  }
}

void loadInstructions() {
  if (Serial1.available()) {
    char instructionsString[200];
    int insArray[MAX_NUM_INST_LOAD], durArray[MAX_NUM_INST_LOAD], numInstructions = 0;
    uint8_t insSize = 0;
    loadInstructionsChunk(instructionsString, &insSize);

    if (insSize > 0) {
      separateInstructions(instructionsString, insArray, durArray, &numInstructions);
      /*
          //Serial.println(numInstructions);
          char printIns[200] = "";
          sprintf(printIns, "%d %d", insArray[0], durArray[0]);
          for (int i = 1; i < numInstructions; i++) {
            sprintf(printIns, "%s, %d %d", printIns, insArray[i], durArray[i]);
          }
          Serial.println(printIns);
      */
      if (sendIntrucctionstoQueue(insArray, durArray, numInstructions) > 0) {
        Serial.println("Fallo al insertar instrucciones");
      }
    }
  }
}

//Aqui se lee un chunk de instrucciones compuesta como máximo por 20 instrucciones
void loadInstructionsChunk(char* instructions, uint8_t* insSize) {
  char inChar;
  uint8_t index = 0;

  while (1) {
    if (Serial1.available()) {
      // Leer un caracter del puerto serie
      inChar = Serial1.read();

      // Verificar si se ha alcanzado el final de la cadena
      if (inChar == '\0') {
        instructions[index] = '\0';  // Agregar el caracter nulo al final de la cadena
        break;  // Salir del bucle
      }
      // Verificar si se ha encontrado un salto de línea
      else if (inChar == '\n') {
        instructions[index] = '\n';  // Agregar el salto de línea a la cadena
        (*insSize)++;  // Incrementar el contador de líneas
      }
      // Agregar el caracter leído a la cadena
      else {
        instructions[index] = inChar;
      }

      index++;  // Incrementar el índice de la cadena
    }
  }
}

void separateInstructions(const char* instructions, int* array1, int* array2, int* numInstructions) {
  char* token;
  char* line;
  char* rest;

  // Hacer una copia de la cadena de instrucciones para evitar modificar la cadena original
  char instructionsCopy[strlen(instructions) + 1];
  strcpy(instructionsCopy, instructions);

  // Inicializar los arrays y el número de instrucciones
  *numInstructions = 0;
  memset(array1, 0, sizeof(int) * MAX_NUM_INST_LOAD);
  memset(array2, 0, sizeof(int) * MAX_NUM_INST_LOAD);

  // Obtener la primera línea
  line = strtok_r(instructionsCopy, "\n", &rest);

  // Procesar cada línea
  while (line != NULL) {
    // Analizar la línea para extraer los dos números
    int num1, num2;
    sscanf(line, "%d %d", &num1, &num2);

    // Almacenar los números en los arrays
    array1[*numInstructions] = num1;
    array2[*numInstructions] = num2;

    // Incrementar el número de instrucciones
    (*numInstructions)++;

    // Obtener la siguiente línea
    line = strtok_r(NULL, "\n", &rest);
  }
}

int sendIntrucctionstoQueue(int* insArray, int* durArray, const int numInstructions) {
  // Validar los valores en el array insArray
  for (int i = 0; i < numInstructions; ++i) {
    if (insArray[i] < 0 || insArray[i] > 5) {
      Serial.print("Fallo en la instruccion (codigo): ");
      Serial.print(insArray[i]);
      Serial.print(" ");
      Serial.print(durArray[i]);
      return 1;
    }
  }

  // Validar los valores en el array durArray
  for (int i = 0; i < numInstructions; ++i) {
    if (durArray[i] < 1 || durArray[i] > 99) {
      Serial.print("Fallo en la instruccion (tiempo): ");
      Serial.print(insArray[i]);
      Serial.print(" ");
      Serial.print(durArray[i]);
      return 1;
    }
  }

  //Una vez se han validado los valores de las instrucciones se almacenan en la cola
  int i = 0;

  //Meter las instrucciones en el array de pasos
  //Si el hueco esta libre se carga la nueva instruccion, si no, se manda un mensaje de error de cola saturada
  while (i < numInstructions && (timepasos[(puntPaso + i) % MAX_NUM_INST] == 0 || timepasos[(puntPaso + i + 1) % MAX_NUM_INST] == 0)) {
    pasos[(puntPaso + i) % MAX_NUM_INST] = insArray[i];
    timepasos[(puntPaso + i) % MAX_NUM_INST] = ((float)durArray[i]) / 10.0;

    i++;
  }
  //Incrementar puntero de la cola de instrucciones
  puntPaso = (puntPaso + i) % MAX_NUM_INST;

  //Imprimir estado actual de la cola de instrucciones
  //printStates();

  //Si el puntero no ha llegado al final significa que la cola se ha saturado
  if (i < numInstructions) {
    Serial.print("Cola saturada. Se omitiran las ultimas ");
    Serial.print(numInstructions - i);
    Serial.println(" instrucciones.");
    return 1;
  }

  // Si el proceso ha salido correctamente devuelve 0
  return 0;
}

void printStates() {
  for (int i = 0; i < MAX_NUM_INST; i++) {
    Serial.print(pasos[i]);
    Serial.print(" ");
  }
  Serial.println("");

  for (int i = 0; i < MAX_NUM_INST; i++) {
    Serial.print((int)timepasos[i]);
    Serial.print(" ");
  }
  Serial.println("");
  Serial.println("------------------");
}
