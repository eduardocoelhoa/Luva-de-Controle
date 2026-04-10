#ifndef SENSORMPU_H
#define SENSORMPU_H

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>

class SensorMPU {
private:
    MPU6050 mpu;
    uint8_t p_sda, p_scl;

public:
    SensorMPU(uint8_t sda, uint8_t scl);
    void begin();
    void update();
    int getEixoX();
    int getEixoY();
};

#endif