#include "MotorDC.h"

MotorDC::MotorDC(uint8_t en, uint8_t in1, uint8_t in2) {
    p_en = en; p_in1 = in1; p_in2 = in2;
}

void MotorDC::begin() {
    pinMode(p_en, OUTPUT);
    pinMode(p_in1, OUTPUT);
    pinMode(p_in2, OUTPUT);
    setVelocidade(0);
}

void MotorDC::setVelocidade(int velocidade) {
    // Define o sentido de rotação
    if (velocidade > 0) { 
        digitalWrite(p_in1, HIGH); 
        digitalWrite(p_in2, LOW); 
    } 
    else if (velocidade < 0) { 
        digitalWrite(p_in1, LOW); 
        digitalWrite(p_in2, HIGH); 
    } 
    else { 
        digitalWrite(p_in1, LOW); 
        digitalWrite(p_in2, LOW); 
    }
    // Aplica o PWM no pino Enable correspondente
    analogWrite(p_en, abs(velocidade));
}