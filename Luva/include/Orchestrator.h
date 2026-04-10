#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include "SensorMPU.h"
#include "EspNow.h"

class Orchestrator {
private:
    SensorMPU sensor;
    EspNow radio;

public:
    Orchestrator(uint8_t pinSda, uint8_t pinScl, const uint8_t* macCarro);
    void begin();
    void loop();
};

#endif