#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include "SensorMPU.h"
#include "EspNow.h"

class Orchestrator {
    
private: 

    SensorMPU sensor;
    EspNow espnow;

    static const int MOVING_AVERAGE_WINDOW = 5;
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