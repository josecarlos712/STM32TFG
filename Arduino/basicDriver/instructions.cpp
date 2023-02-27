#include "HardwareSerial.h"
#include "string.h"
#include "instructions.hpp"
#include "globalConst.hpp"

//Variables globales
extern int paso, puntPaso;
extern int pasos[MAX_NUM_INST];
extern float timepasos[MAX_NUM_INST];
//Variables locales
char inChar;
String s = "";
bool insComp = false;
int tempPuntPaso = 0;

void initTest() {
  int seg = 1;
  //  int temppasos[20] = {1, 1, 2, 2, 3, 4, 1, 5, 1, 1, 2, 2, 3, 4, 1, 5, 0, 0, 0, 0}; //20 elementos
  //  float temptimepasos[20] = {3.0, 2.0, 1.0, 1.0, 3.0, 2.0, 1.0, 5.0, .0, 2.0, 1.0, 1.0, 3.0, 2.0, 1.0, 5.0, 0.0, 0.0, 0.0, 0.0}; //20 elementos
  int temppasos[20] = {1, 1, 2, 2, 3, 4, 1, 5, 1, 1, 2, 2, 3, 4, 1, 5, 0, 0, 0, 0}; //20 elementos
  //float temptimepasos[20] = {3.0, 2.0, 1.0, 1.0, 3.0, 2.0, 1.0, 0.0, 0.0, 3.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; //20 elementos
  float temptimepasos[20] = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; //20 elementos

  for (int i = 0; i < MAX_NUM_INST; i++) {
    pasos[i] = 0;
    timepasos[i] = 0;
  }
}

//str - instruccion a separar
//strs - puntero a array donde se guardara la instruccion separada
//N - numero de instrucciones maximas esperadas (o tamaño del array strs entre dos)
void splitInstructions(String str, String* strs, int N) {
  int StringCount = 0;
  // 1 30 2 8 3 20 4 9 5 45 1 14 2 23

  // Separa el string en substrings
  while (str.length() > 0 && StringCount < N * 2)
  {
    int index = str.indexOf(' ');
    if (index == -1) // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index + 1);
    }
  }
  /*
    for (int i = 0;i<StringCount;i++) {
      Serial.print("I");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(strs[i]);
    }
  */
}

void cleanQueue() {
  for (int i = 0; i < MAX_NUM_INST; i++) {
    pasos[i] = 0;
    timepasos[i] = 0.0;
  }
}

void test() {
  cleanQueue();
}

// Carga una sola instruccion pero requiere de varias llamadas para completarse ya que va leyendo caracter a caracter.
void loadInstructions() {
  //Lectura de instrucciones por el puerto serie
  if (Serial1.available()) {
    //Va leyendo un caracter y agregandolo al string hasta que se encuentra con un salto de linea que finaliza la instruccion
    inChar = Serial1.read();
    if (inChar == 10/*(salto de linea en ASCII)*/) {
      Serial.println(s);
      insComp = true;
    } else {
      s += inChar;
    }
    //Se limpia el puerto serie
    Serial1.flush();

    //Si la isntruccion esta completa se agrega a la cola de instrucciones
    if (insComp) {
      Serial.print("Recibido: ");
      Serial.println(s);

      //Separar la instruccion en instrucciones las dos componentes
      String strPasos[2];
      splitInstructions(s, strPasos, 1);
      //Se actualiza el puntero de la cola de instrucciones
      tempPuntPaso = puntPaso % MAX_NUM_INST;

      //Meter las instrucciones en el array de pasos
      //Si el hueco esta libre se carga la nueva instruccion sino se manda un mensaje de error de cola saturada
      if (timepasos[tempPuntPaso] == 0 || timepasos[(tempPuntPaso + 1) % MAX_NUM_INST] == 0) {
        pasos[tempPuntPaso] = strPasos[0].toInt();
        timepasos[tempPuntPaso] = ((float)strPasos[1].toInt()) / 10.0;
      }  else {
        Serial.print("Valor de cola: ");
        Serial.println(pasos[tempPuntPaso]);
        Serial.println("Cola saturada. Se omitira la ultima instruccion.");
      }

      //Imprimir estado actual de la cola de instrucciones
      printStates();
      //Incrementar puntero de la cola de instrucciones
      puntPaso++;
      //Se resetea el la instruccion para que entre una nueva
      s = "";
      insComp = false;
    }
  }
}

// Para cargar varias instrucciones que venga en una misma linea
// Para ello es necesario separar cada una de las instrucciones
void loadNInstructions(int N) {
  N = N > 0 ? N : 0; //N debe ser > 0
  N = N <= MAX_NUM_INST ? N : MAX_NUM_INST; //Y debe ser < MAX_NUM_INST
  //Lectura de instrucciones por el puerto serie
  if (Serial1.available()) {
    //Va leyendo un caracter y agregandolo al string hasta que se encuentra con un salto de linea que finaliza la instruccion
    // y la agrega a la cola de ejecucion
    inChar = Serial1.read();
    if (inChar == 10/*(salto de linea en ASCII)*/) {
      //Serial.println(s);
      insComp = true;
      s = "";
    } else {
      insComp = false;
      s += inChar;
    }
    Serial1.flush();

    if (insComp) {
      Serial.print("Recibido: ");
      Serial.println(s);

      //Separar la linea de instrucciones en instrucciones simples
      String strPasos[MAX_NUM_INST * 2];
      splitInstructions(s, strPasos, N);

      for (int i = 0; i < N * 2; i++) {
        int p = strPasos[i].toInt();
        tempPuntPaso = puntPaso % MAX_NUM_INST;
        //Meter las instrucciones en el array de pasos
        if (i % 2 == 0) {
          if (pasos[tempPuntPaso] == 0) { //Si el hueco esta libre se carga la nueva instruccion sino se manda un mensaje de error de cola saturada
            pasos[tempPuntPaso] = p;
          } else if (pasos[(tempPuntPaso + 1) % MAX_NUM_INST] == 0) {
            pasos[tempPuntPaso] = p;
          } else {
            Serial.print("Valor de cola: ");
            Serial.println(pasos[tempPuntPaso]);
            Serial.print("Cola saturada. Se omitiran las instrucciones a partir de la numero ");
            Serial.println(i / 2);
            break;
          }
        } else {
          timepasos[tempPuntPaso] = ((float)p) / 10.0;
          puntPaso++;
        }
      }
      printStates();
    }
  }
}

void printStates() {
  for (int i = 0; i < MAX_NUM_INST - 1; i++) {
    Serial.print(pasos[i]);
    Serial.print(".00 ");
  }
  Serial.println(pasos[MAX_NUM_INST - 1]);

  for (int i = 0; i < MAX_NUM_INST - 1; i++) {
    Serial.print(timepasos[i]);
    Serial.print(" ");
  }
  Serial.println(timepasos[MAX_NUM_INST - 1]);
  Serial.println("------------------");
}