#define INSTR_HEAD
#include "driver.h"
#include "instructions.hpp"
#include "basicDriver.hpp"
#include "globalConst.hpp"

//Constantes
int paso = 0, puntPaso = 0;
int pasos[MAX_NUM_INST];
float timepasos[MAX_NUM_INST];

void setup()
{
  pinMode(50, INPUT_PULLUP);
  pinMode(48, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(vcc, OUTPUT);
  pinMode(gnd, OUTPUT);
  digitalWrite(48, LOW);
  digitalWrite(gnd, LOW);
  digitalWrite(vcc, HIGH);
  Serial1.begin(9600);
  Serial.begin(9600);
  Serial.setTimeout(30);

  //Iniciacion de variables
  initCont = micros(); //Bandera de tiempo para el contador del escaner
  cleanQueue();
}

void loop()
{
  if (digitalRead(50) == LOW) {
    if (primera) {
      primera = false;
    }
    loadInstructions();
    //scanner();
    continueMov();
  }
}

void scanner() {
  //Escaner de proximidad
  prevAcumCont = acumCont;
  acumCont += micros() - initCont - prevAcumCont;
  if (acumCont >= 1000000 / NUM_SCANS_PER_SECOND) {
    acumCont -= 1000000 / NUM_SCANS_PER_SECOND;
    initCont = micros();
    checkFrontWall();
  }
  if (!cerca) {
    //Serial.print(cerca);
    continueMov();
  }
}


void checkFrontWall() {
  // Clears the trig condition
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trig HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Reads the echo, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  if (distance < 200) { //Proteccion para valores anormales
    if (distance < 10 && !cerca) { //Si la distancia es menor a 10cm el vehiculo se para
      cerca = true;
      Serial.println("PARA");
      para();
    } else if (distance > 30 && cerca) { //Si la distancia aumenta a 30 el vehiculo continua el trayecto
      if (initStop == 0) {
        initStop = micros();
      }
      acumStop = micros() - initStop;
      cerca = false;
      initmicro = micros() - microCrono; //Una vez se reanuda el trayecto se establece un nuevo inicio en base a cuanto llevaba ya ejecutado
      //Serial.println("SIGUE");
    }
  }
}

void doMove(const int est, const float dur) {
  switch (est) {
    case PARADO:
      estado = PARADO;
      microCrono = 0;
      goalCrono = dur * 1000000;
      Serial.println("Parado");
      if (enableMovement) {
        para();
      }
      break;
    case ADELANTE:
      estado = ADELANTE;
      microCrono = 0;
      goalCrono = dur * 1000000;
      Serial.println("Adelante");
      if (enableMovement) {
        adelante();
      }
      break;
    case ATRAS:
      estado = ATRAS; //Se carga el estado
      microCrono = 0; //Se reinicia el cronometro de estado
      goalCrono = dur * 1000000;
      Serial.println("Atras");
      if (enableMovement) {
        atras();
      }
      break;
    case DERECHA:
      estado = DERECHA;
      microCrono = 0;
      goalCrono = dur * 1000000;
      Serial.println("Derecha");
      if (enableMovement) {
        girarDer();
      }
      break;
    case IZQUIERDA:
      estado = IZQUIERDA;
      microCrono = 0;
      goalCrono = dur * 1000000;
      Serial.println("Izquierda");
      if (enableMovement) {
        girarIzq();
      }
      break;
    case NOPE:
      estado = NOPE;
      microCrono = 0;
      goalCrono = dur * 1000000;
      Serial.println("NOPE");
      break;
    default:
      microCrono = 0;
      goalCrono = 1;
      Serial.println("Parado");
      para();
      break;
  }
}

void continueMov() {
  microCrono = micros() - initmicro;
  if (goalCrono >= 0) {
    if (microCrono >= goalCrono) {
      siguienteMov();
    }
  }
}

void siguienteMov() {
  initmicro = micros();
  if (paso >= 0 and paso < MAX_NUM_INST) {

    int tempEstado = pasos[paso];
    float tempTimeEstado = timepasos[paso];
    pasos[(paso - 1) % MAX_NUM_INST] = 0; //Se borra el paso anterior y su tiempo
    timepasos[(paso - 1) % MAX_NUM_INST] = 0.0;

    if (tempEstado >= 0 && tempTimeEstado > 0.0) { // Se comprueba si tanto como el estado como el tiempo de estado tienen valores correctos
      doMove(tempEstado, tempTimeEstado); //Se hace el movimiento correspondiente
    }

    paso = (paso + 1) % MAX_NUM_INST;
  } else {
    Serial.println("PARADA DE EMERGENCIA. Error en el valor de estado o tiempo de ejecucion");
    paso = -1; // Cuando termina todos los pasos en contador vuelve a -1 para que no siga
    estado = PARADO;
    para();
  }
}
