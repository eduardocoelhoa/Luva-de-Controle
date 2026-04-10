#ifndef ESPNOW_H
#define ESPNOW_H

#include <Arduino.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include "CarData.h"

class EspNow {
private:
    uint8_t macDestino[6];

public:
    EspNow(const uint8_t* mac);
    void begin();
    void sendData(CarData dados);
};

#endif