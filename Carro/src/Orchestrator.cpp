#include "Orchestrator.h"

// Inicializa o chassi com os pinos: ENA(14), ENB(27), IN1(5), IN2(18), IN3(2), IN4(4)
Orchestrator::Orchestrator(): chassi(14, 27, 5, 18, 2, 4), ultrassonic(19, 23) {}

void Orchestrator::begin() {
    chassi.begin();
    comms.begin();
    Serial.println("Orquestrador iniciado com sucesso. Aguardando Luva...");
}

void Orchestrator::loop() {
    // 1. Verifica Failsafe (Segurança)
    if (millis() - comms.getLastRecvTime() > 500) {
        chassi.stop();
    }
    // 2. Transfere os dados do rádio para o chassi
    else {
        CarData dados = espnow.getData();
        chassi.driveArcade(dados.x, dados.y);
    }
}