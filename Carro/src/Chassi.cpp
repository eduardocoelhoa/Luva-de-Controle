#include "Chassi.h"

// Inicializa os objetos MotorDC na lista de inicialização
Chassi::Chassi(uint8_t ena, uint8_t enb, uint8_t in1, uint8_t in2, uint8_t in3, uint8_t in4) 
    : motorEsquerdo(ena, in1, in2), motorDireito(enb, in3, in4) {}

void Chassi::begin() {
    motorEsquerdo.begin();
    motorDireito.begin();
}

void Chassi::driveArcade(int x, int y) {
    // 1. Mapeamento
    int aceleracao = map(x, -60, 60, -255, 255);
    int direcao    = map(y, -60, 60, -255, 255);

    // 2. Zona Morta
    if (abs(aceleracao) < 30) aceleracao = 0;
    if (abs(direcao) < 30) direcao = 0;

    // 3. Mistura dos eixos (Arcade Steering)
    int velEsq = constrain(aceleracao + direcao, -255, 255);
    int velDir = constrain(aceleracao - direcao, -255, 255);

    // 4. Delega a ação
    motorEsquerdo.setVelocidade(velEsq);
    motorDireito.setVelocidade(velDir);
}

void Chassi::stop() {
    driveArcade(0, 0);
}