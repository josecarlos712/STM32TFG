#include "driver.h"
#include <AFMotor.h>

AF_DCMotor Motor1(1);
AF_DCMotor Motor2(2);
AF_DCMotor Motor3(3);
AF_DCMotor Motor4(4);

void girarDer() {
    Motor3.setSpeed(255);
    Motor4.setSpeed(255);
    Motor1.setSpeed(255);
    Motor2.setSpeed(255);
    Motor3.run(FORWARD);
    Motor4.run(FORWARD);
    Motor1.run(BACKWARD);
    Motor2.run(BACKWARD);
}

void girarIzq() {
    Motor1.setSpeed(255);
    Motor2.setSpeed(255);
    Motor3.setSpeed(255);
    Motor4.setSpeed(255);
    Motor1.run(FORWARD);
    Motor2.run(FORWARD);
    Motor3.run(BACKWARD);
    Motor4.run(BACKWARD);
}

void para() {
    Motor1.run(RELEASE);
    Motor2.run(RELEASE);
    Motor3.run(RELEASE);
    Motor4.run(RELEASE);
}

void atras() {
    Motor1.setSpeed(200);
    Motor2.setSpeed(200);
    Motor3.setSpeed(200);
    Motor4.setSpeed(200);
    Motor1.run(BACKWARD);
    Motor2.run(BACKWARD);
    Motor3.run(BACKWARD);
    Motor4.run(BACKWARD);
}

void adelante() {
    Motor1.setSpeed(200);
    Motor2.setSpeed(200);
    Motor3.setSpeed(200);
    Motor4.setSpeed(200);
    Motor1.run(FORWARD);
    Motor2.run(FORWARD);
    Motor3.run(FORWARD);
    Motor4.run(FORWARD);
}
