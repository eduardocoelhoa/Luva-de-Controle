#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include "Chassi.h"
#include "EspNow.h"
#include "OtaService.h"

class Orchestrator {
private:
    Chassi chassi;
    EspNowManager comms;
    OtaService otaService;
    bool coreStarted = false;
    unsigned long lastLoopMs = 0;

    static constexpr unsigned long kLoopIntervalMs = 20UL;

public:
    Orchestrator();
    void begin();
    void loop();
};

#endif