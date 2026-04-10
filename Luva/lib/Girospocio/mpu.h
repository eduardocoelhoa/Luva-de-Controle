#ifndef MPU_H
#define MPU_H

#include "Arduino.h"
#include "Wire.h"
#include "Define.h"

class MPU6050 {
  public:
    // Construtor: aceita a interface Wire (padrão ou Wire1, etc)
    // accCoef e gyroCoef ajustam o peso do filtro complementar (padrão 0.02 e 0.98)
    MPU6050(TwoWire &w, float accCoef = 0.02f, float gyroCoef = 0.98f);
    void begin();
    void update();
    
    // Calibra o giroscópio (o sensor deve ficar imóvel durante isso)
    void calibrate(bool consoleOutput = true);

    // --- Getters de Dados Processados (Ângulos) ---
    float getAngleX() const { return angleX; } // Roll (rotação lateral)
    float getAngleY() const { return angleY; } // Pitch (inclinação frente/trás)
    float getAngleZ() const { return angleZ; } // Yaw (rotação em torno do próprio eixo)

    // --- Getters de Aceleração (Normalizados em g) ---
    float getAccX() const { return accX; }
    float getAccY() const { return accY; }
    float getAccZ() const { return accZ; }

    // --- Getters de Giroscópio (Normalizados em graus/segundo) ---
    float getGyroX() const { return gyroX; }
    float getGyroY() const { return gyroY; }
    float getGyroZ() const { return gyroZ; }

    // --- Getters de Temperatura ---
    float getTemp() const { return temperature; }

  private:
    TwoWire *wire;
    float accCoef, gyroCoef;
    
    // Dados brutos
    int16_t rawAccX, rawAccY, rawAccZ;
    int16_t rawGyroX, rawGyroY, rawGyroZ;
    int16_t rawTemp;

    // Offsets de calibração
    float gyroXoffset, gyroYoffset, gyroZoffset;

    // Dados convertidos
    float accX, accY, accZ;
    float gyroX, gyroY, gyroZ;
    float temperature;

    // Ângulos calculados
    float angleAccX, angleAccY;
    float angleX, angleY, angleZ;

    // Controle de tempo
    long preInterval;

    // Métodos internos de comunicação I2C
    void writeRegister(byte reg, byte data);
    byte readRegister(byte reg);
};

#endif