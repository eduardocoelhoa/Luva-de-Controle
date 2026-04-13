#include <Arduino.h>
#include "Orchestrator.h"

// MAC Address do receptor (Carrinho)
const uint8_t macDoCarro[] = {0x5C, 0x01, 0x3B, 0x9D, 0x74, 0x2C};

// Instancia o orquestrador com os pinos SDA(21), SCL(22) e o MAC
Orchestrator controleLuva(21, 22, macDoCarro);

void setup() {
    Serial.begin(115200);
    controleLuva.begin();
}

void loop() {
    controleLuva.loop();
    delay(50); // Delay mantido para não saturar a rede Wi-Fi
}