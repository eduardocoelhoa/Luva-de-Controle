#include "Orchestrator.h"

// Inicializa o chassi com os pinos: ENA(14), ENB(27), IN1(2), IN2(4), IN3(5), IN4(18)
// *Ajuste os pinos 14 e 27 aqui conforme a sua montagem real!*
Orchestrator::Orchestrator() : chassi(14, 27, 2, 4, 5, 18) {} 

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
        CarData dados = comms.getData();
        chassi.driveArcade(dados.x, dados.y);
    }
}