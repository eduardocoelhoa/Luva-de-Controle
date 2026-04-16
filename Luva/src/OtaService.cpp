#include "OtaService.h"

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <cstring>

#include "OtaConfig.h"

void OtaService::begin() {
    if (!OtaConfig::kEnable) {
        state = State::Disabled;
        Serial.println("OTA desativado (build atual).");
        return;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(OtaConfig::kWifiSsid, OtaConfig::kWifiPassword);

    connectStartMs = millis();
    lastHandleMs = 0;
    state = State::Connecting;
    Serial.println("OTA: conectando Wi-Fi em background...");
}

void OtaService::update() {
    if (state == State::Disabled || state == State::Failed) {
        return;
    }

    if (state == State::Connecting) {
        if (WiFi.status() == WL_CONNECTED) {
            startArduinoOta();
            return;
        }

        if (millis() - connectStartMs >= OtaConfig::kConnectTimeoutMs) {
            state = State::Failed;
            Serial.println("OTA indisponivel: timeout no Wi-Fi.");
        }
        return;
    }

    if (state == State::Ready && millis() - lastHandleMs >= OtaConfig::kHandleIntervalMs) {
        lastHandleMs = millis();
        ArduinoOTA.handle();
    }
}

bool OtaService::isResolved() const {
    return state == State::Disabled || state == State::Ready || state == State::Failed;
}

bool OtaService::isReady() const {
    return state == State::Ready;
}

void OtaService::startArduinoOta() {
    ArduinoOTA.setHostname(OtaConfig::kHostname);

    if (std::strlen(OtaConfig::kPassword) > 0) {
        ArduinoOTA.setPassword(OtaConfig::kPassword);
    }

    ArduinoOTA.onStart([]() {
        Serial.println("Iniciando OTA...");
    });

    ArduinoOTA.onEnd([]() {
        Serial.println("OTA finalizado.");
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("OTA: %u%%\r", (progress * 100U) / total);
    });

    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Erro OTA [%u]\n", error);
    });

    ArduinoOTA.begin();

    state = State::Ready;
    Serial.print("OTA pronto em: ");
    Serial.println(WiFi.localIP());
}
