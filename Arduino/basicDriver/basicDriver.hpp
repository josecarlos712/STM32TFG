//TO-DO
//Cuando le envias instrucciones va metiendolas hasta que no haya hueco. Cuando no hay hueco se manda un mensaje de "FULL".
//Cada vez que se termina una instruccion se borra de la colo para que el hueco quede libre
//Cuando puntPaso llega al final empieze desde el principio y se mueva al siguiente solo si el hueco esta libre.

#ifndef MAIN_FUNC
#define MAIN_FUNC

//Control de flujo
bool primera = true, enableMovement = false;

//Control de estado
int const NOPE = 0, ADELANTE = 1, ATRAS = 2, DERECHA = 3, IZQUIERDA = 4, PARADO = 5;
int estado = PARADO;
long initmicro = 0, microCrono = 0, goalCrono = 0, prevMicroCrono = 0;

//Control de sensor proximidad
int trig = 45, echo = 40, vcc = 44, gnd = 42;
long duration;
int distance;
bool cerca = false;
//Control de parada por cercania
long initStop = 0, acumStop = 0;

//Test conteo escaner
int scanTimes = 0;
long initCont = 0, acumCont = 0, prevAcumCont = 0;

//Declaracion de funciones
void scanner();
void doMove(int, float);
void checkFrontWall();
void continueMov();
void siguienteMov();
#endif
