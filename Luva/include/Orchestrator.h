#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include "SensorMPU.h"
#include "EspNow.h"
#include "OtaService.h"

class Orchestrator {
private:

    SensorMPU sensor;
    EspNow espnow;
    OtaService otaService;
    bool coreStarted = false;
    unsigned long lastLoopMs = 0;

    static const int MOVING_AVERAGE_WINDOW = 5;
    static constexpr unsigned long kLoopIntervalMs = 50UL;
    int bufferX[MOVING_AVERAGE_WINDOW] = {0};
    int bufferY[MOVING_AVERAGE_WINDOW] = {0};
    int indexX = 0;
    int indexY = 0;
    int countX = 0;
    int countY = 0;

    int movingAverage(int valor, int* buffer, int& index, int& count);

public:
    Orchestrator(uint8_t pinSda, uint8_t pinScl, const uint8_t* macCarro);
    void begin();
    void loop();
};

#endif