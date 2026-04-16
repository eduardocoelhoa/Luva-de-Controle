#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include "Chassi.h"
#include "EspNow.h"
#include "Ultrassonic.h"

class Orchestrator {

    private:
        Chassi chassi;
        EspNow espnow;
        Ultrassonic ultrassonic;

    public:
        Orchestrator();
        void begin();
        void loop();};

#endif