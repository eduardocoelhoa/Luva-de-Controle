#ifndef CHASSI_H
#define CHASSI_H

#include "MotorDC.h"

class Chassi {
private:
    MotorDC motorEsquerdo;
    MotorDC motorDireito;

public:
    Chassi(uint8_t ena, uint8_t enb, uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4);
    void begin();
    void driveArcade(int x, int y);
    void stop();
};

#endif