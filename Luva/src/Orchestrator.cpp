#include "Orchestrator.h"

Orchestrator::Orchestrator(uint8_t pinSda, uint8_t pinScl, const uint8_t* macCarro)
    : sensor(pinSda, pinScl), radio(macCarro) {}

void Orchestrator::begin() {
    sensor.begin(); // O sensor vai calibrar aqui
    radio.begin();
    Serial.println("Luva pronta e orquestrador iniciado!");
}

void Orchestrator::loop() {
    // 1. Atualiza leituras
    sensor.update();

    // 2. Monta o pacote
    CarData dados;
    dados.x = sensor.getEixoX();
    dados.y = sensor.getEixoY();

    // 3. Imprime para debug
    Serial.print("X: "); Serial.print(dados.x);
    Serial.print(" | Y: "); Serial.println(dados.y);

    // 4. Envia via rádio
    radio.sendData(dados);
}