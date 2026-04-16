#include "Orchestrator.h"

Orchestrator::Orchestrator(uint8_t pinSda, uint8_t pinScl, const uint8_t* macCarro)
    : sensor(pinSda, pinScl), espnow(macCarro) {}

void Orchestrator::begin() {
    otaService.begin();
}

int Orchestrator::movingAverage(int valor, int* buffer, int& index, int& count) {
    const int n = MOVING_AVERAGE_WINDOW;

    buffer[index] = valor;        // Insere o novo valor no buffer
    index = (index + 1) % n;      // Move o índice, voltando ao início se necessário
    if (count < n) count++;       // Incrementa o contador até atingir n

    // Calcula a média dos valores no buffer
    int soma = 0;
    for (int i = 0; i < count; i++) {
        soma += buffer[i];
    }
    return soma / count;          // Retorna a média
}

void Orchestrator::loop() {
    otaService.update();

    if (!coreStarted && otaService.isResolved()) {
        sensor.begin(); // O sensor vai calibrar aqui
        espnow.begin();
        coreStarted = true;
        Serial.println("Luva pronta e orquestrador iniciado!");
    }

    if (!coreStarted) {
        return;
    }

    const unsigned long now = millis();
    if (now - lastLoopMs < kLoopIntervalMs) {
        return;
    }
    lastLoopMs = now;

    // 1. Atualiza leituras
    sensor.update();

    // 2. Monta o pacote
    CarData dados;
    dados.x = movingAverage(sensor.getEixoX(), bufferX, indexX, countX);
    dados.y = movingAverage(sensor.getEixoY(), bufferY, indexY, countY);

    // 3. Imprime para debug
    Serial.print("X: "); Serial.print(dados.x);
    Serial.print(" | Y: "); Serial.println(dados.y);

    // 4. Envia via rádio
    espnow.sendData(dados);
}