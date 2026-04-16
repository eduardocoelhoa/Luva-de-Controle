#ifndef ESPNOWMANAGER_H
#define ESPNOWMANAGER_H

#include <Arduino.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include "CarData.h"


class EspNow {

    public:
        void begin();
        CarData getData();
        unsigned long getLastRecvTime();
};

#endif