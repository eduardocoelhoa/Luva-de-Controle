#include "EspNow.h"

// Estado simulado privado
static CarData dadosAtuais = {0, 0};
static unsigned long tempoUltimoPacote = 0;

// Callback livre
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
    memcpy(&dadosAtuais, incomingData, sizeof(dadosAtuais));
    tempoUltimoPacote = millis();
}

void EspNow::begin() {
    WiFi.mode(WIFI_STA);
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Erro Crítico: Falha no ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);
}

CarData EspNow::getData() {
    return dadosAtuais;
}

unsigned long EspNow::getLastRecvTime() {
    return tempoUltimoPacote;
}