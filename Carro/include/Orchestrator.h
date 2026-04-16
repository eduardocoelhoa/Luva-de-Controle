#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include "Chassi.h"
#include "EspNow.h"

class Orchestrator {
private:
    Chassi chassi;
    EspNowManager comms;

    public:
        Orchestrator();
        void begin();
        void loop();};

#endif