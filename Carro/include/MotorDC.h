#ifndef MOTORDC_H
#define MOTORDC_H

#include <Arduino.h>

class MotorDC {
private:
    uint8_t p_en, p_in1, p_in2;

public:
    MotorDC(uint8_t en, uint8_t in1, uint8_t in2);
    void begin();
    void setVelocidade(int velocidade); // Recebe de -255 a 255
};

#endif