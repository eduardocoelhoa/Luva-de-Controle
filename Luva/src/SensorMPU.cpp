#include "SensorMPU.h"

// O construtor inicializa a variável do MPU com a classe Wire (padrão I2C)
SensorMPU::SensorMPU(uint8_t sda, uint8_t scl) : mpu(Wire) {
    p_sda = sda;
    p_scl = scl;
}

void SensorMPU::begin() {
    Wire.begin(p_sda, p_scl);
    mpu.begin();

    digitalWrite(LED_BUILTIN, HIGH); // Acende o LED para indicar que está calibrando
    Serial.println("CALIBRANDO SENSOR...");
    mpu.calcGyroOffsets(true); // Calibra ao ligar
    Serial.println("SENSOR CALIBRADO!");
    digitalWrite(LED_BUILTIN, LOW); // Apaga o LED para indicar que terminou a calibração
}

void SensorMPU::update() {
    mpu.update(); // Atualiza as leituras internas
}

int SensorMPU::getEixoX() {
    return -mpu.getAngleX(); // Mapeia inclinação frente/trás
}

int SensorMPU::getEixoY() {
    return -mpu.getAngleY(); // Mantido o sinal negativo original do seu código para inverter a direção [cite: 8]
}