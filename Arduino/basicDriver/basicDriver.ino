#define INSTR_HEAD
#include "driver.h"
#include "instructions.hpp"
#include "basicDriver.hpp"
#include "globalConst.hpp"

//Constantes
int paso = 0;
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
  //initTest();
}

void loop()
{
  if (digitalRead(50) == LOW) {
    scanner();
    continueMov();
  }
  //loadInstructions();
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
  // Borra la condición de disparo
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Establece trig en ALTO (ACTIVO) durante 10 microsegundos
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  // Lee el eco, devuelve el tiempo de viaje de la onda de sonido en microsegundos
  duration = pulseIn(echo, HIGH);
  // Calcula la distancia
  distance = duration * 0.034 / 2; // Velocidad de la onda de sonido dividida por 2 (ida y vuelta)
  if (distance < 200) { // Protección para valores anormales
    if (distance < 10 && !cerca) { // Si la distancia es menor a 10 cm, el vehículo se detiene
      cerca = true;
      Serial.println("PARA");
      para();
    } else if (distance > 30 && cerca) { // Si la distancia aumenta a 30 cm, el vehículo continúa su trayecto
      if (initStop == 0) {
        initStop = micros();
      }
      acumStop = micros() - initStop;
      cerca = false;
      initmicro = micros() - microCrono; // Una vez se reanuda el trayecto, se establece un nuevo inicio basado en el tiempo ya ejecutado
      // Serial.println("SIGUE");
    }
  }
}

void doMove(const int est, const float dur) {
  switch (est) {
    case PARADO:
      estado = PARADO;
      microCrono = 0;
      goalCrono = dur * 1000000;
      Serial.print("Parado ");
      Serial.println(dur);
      if (enableMovement) {
        para();
      }
      break;
    case ADELANTE:
      estado = ADELANTE;
      microCrono = 0;
      goalCrono = dur * 1000000;
      Serial.print("Adelante ");
      Serial.println(dur);
      if (enableMovement) {
        adelante();
      }
      break;
    case ATRAS:
      estado = ATRAS; //Se carga el estado
      microCrono = 0; //Se reinicia el cronometro de estado
      goalCrono = dur * 1000000;
      Serial.print("Atras ");
      Serial.println(dur);
      if (enableMovement) {
        atras();
      }
      break;
    case DERECHA:
      estado = DERECHA;
      microCrono = 0;
      goalCrono = dur * 1000000;
      Serial.print("Derecha ");
      Serial.println(dur);
      if (enableMovement) {
        girarDer();
      }
      break;
    case IZQUIERDA:
      estado = IZQUIERDA;
      microCrono = 0;
      goalCrono = dur * 1000000;
      Serial.print("Izquierda ");
      Serial.println(dur);
      if (enableMovement) {
        girarIzq();
      }
      break;
    case NOPE:
      estado = NOPE;
      microCrono = 0;
      goalCrono = dur * 1000000;
      Serial.print("NOPE ");
      Serial.println(dur);
      break;
    default:
      microCrono = 0;
      goalCrono = 1;
      Serial.print("Parado ");
      Serial.println(dur);
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
