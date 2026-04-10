#include "mpu.h"

MPU6050::MPU6050(TwoWire &w, float aC, float gC) {
  wire = &w;
  accCoef = aC;
  gyroCoef = gC;
  gyroXoffset = 0; gyroYoffset = 0; gyroZoffset = 0;
  angleX = 0; angleY = 0; angleZ = 0;
}

void MPU6050::begin(){
        
  //Escreve no registrador de configuração do MPU6050      
  writeRegister(MPU6050_SMPLRT_DIV, 0x00);      // Taxa de amostragem máxima
  writeRegister(MPU6050_CONFIG, 0x00);          // Filtro passa-baixa desativado
  writeRegister(MPU6050_GYRO_CONFIG, 0x08);     // Escala de +/- 500 dps
  writeRegister(MPU6050_ACCEL_CONFIG, 0x00);    // Escala de +/- 2g
  writeRegister(MPU6050_PWR_MGMT_1, 0x01);      // Usa o clock do oscilador interno
  
  // Inicialização inicial dos ângulos baseada apenas no acelerômetro
  this->update();
  angleX = angleAccX;
  angleY = angleAccY;
  
  preInterval = millis();
}

void MPU6050::writeRegister(byte reg, byte data) {
  wire->beginTransmission(MPU6050_ADDR);
  wire->write(reg);
  wire->write(data);
  wire->endTransmission();
}

byte MPU6050::readRegister(byte reg) {
  wire->beginTransmission(MPU6050_ADDR);
  wire->write(reg);
  wire->endTransmission(true);
  wire->requestFrom(MPU6050_ADDR, 1);
  return wire->read();
}

void MPU6050::calibrate(bool consoleOutput) {
  float x = 0, y = 0, z = 0;
  int16_t rx, ry, rz;

  if (consoleOutput) {
    Serial.println(F("Calibrando Giroscopio... Mantenha a luva parada."));
  }
  
  delay(1000); // Pequena pausa antes de começar

  int numSamples = 2000;
  for (int i = 0; i < numSamples; i++) {
    wire->beginTransmission(MPU6050_ADDR);
    wire->write(0x43); // Registrador inicial do Giroscópio
    wire->endTransmission(false);
    wire->requestFrom((int)MPU6050_ADDR, 6);

    rx = wire->read() << 8 | wire->read();
    ry = wire->read() << 8 | wire->read();
    rz = wire->read() << 8 | wire->read();

    x += ((float)rx) / 65.5;
    y += ((float)ry) / 65.5;
    z += ((float)rz) / 65.5;
  }

  gyroXoffset = x / numSamples;
  gyroYoffset = y / numSamples;
  gyroZoffset = z / numSamples;

  if (consoleOutput) {
    Serial.println(F("Calibracao Concluida!"));
  }
}

void MPU6050::update() {
  wire->beginTransmission(MPU6050_ADDR);
  wire->write(0x3B); // Registrador inicial (Accel X)
  wire->endTransmission(false);
  wire->requestFrom((int)MPU6050_ADDR, 14);

  rawAccX = wire->read() << 8 | wire->read();
  rawAccY = wire->read() << 8 | wire->read();
  rawAccZ = wire->read() << 8 | wire->read();
  rawTemp = wire->read() << 8 | wire->read();
  rawGyroX = wire->read() << 8 | wire->read();
  rawGyroY = wire->read() << 8 | wire->read();
  rawGyroZ = wire->read() << 8 | wire->read();

  // Conversões
  temperature = (rawTemp + 12412.0) / 340.0;

  accX = ((float)rawAccX) / 16384.0;
  accY = ((float)rawAccY) / 16384.0;
  accZ = ((float)rawAccZ) / 16384.0;

  gyroX = ((float)rawGyroX) / 65.5;
  gyroY = ((float)rawGyroY) / 65.5;
  gyroZ = ((float)rawGyroZ) / 65.5;

  // Aplica offsets
  gyroX -= gyroXoffset;
  gyroY -= gyroYoffset;
  gyroZ -= gyroZoffset;

  // Cálculo dos ângulos do acelerômetro (Fórmulas Padrão para sensor plano)
  // angleAccX (Roll) e angleAccY (Pitch)
  angleAccX = atan2(accY, accZ) * 180 / PI;
  angleAccY = atan2(-accX, accZ) * 180 / PI;

  // Cálculo do tempo decorrido
  unsigned long currentMillis = millis();
  float interval = (currentMillis - preInterval) * 0.001;
  preInterval = currentMillis;

  // Integração do giroscópio
  angleX += gyroX * interval;
  angleY += gyroY * interval;
  angleZ += gyroZ * interval;

  // Filtro Complementar (Fusão Sensoral)
  // Combina giroscópio (preciso a curto prazo) com acelerômetro (estável a longo prazo)
  angleX = (gyroCoef * (angleX + gyroX * interval)) + (accCoef * angleAccX);
  angleY = (gyroCoef * (angleY + gyroY * interval)) + (accCoef * angleAccY);
  // Z não tem referência gravitacional confiável para correção simples, usa apenas gyro
}