#include <Arduino.h>
#include "Orchestrator.h"

// Instancia o Orquestrador globalmente
Orchestrator roboCarro;

void setup() {
    Serial.begin(115200);
    roboCarro.begin();
}

void loop() {
    roboCarro.loop();
    delay(20);
}