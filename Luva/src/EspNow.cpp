#include "EspNow.h"

// Callback solto para atender à exigência da biblioteca ESP-NOW [cite: 4]
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    // Pode-se adicionar lógica aqui para piscar um LED se falhar, por exemplo
}

EspNow::EspNow(const uint8_t* mac) {
    memcpy(macDestino, mac, 6);
}

void EspNow::begin() {
    WiFi.mode(WIFI_STA);

    if (WiFi.status() != WL_CONNECTED) {
        // Sem Wi-Fi conectado, mantém o canal fixo usado pelo projeto.
        esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
    } else {
        // Com OTA ativo, o canal segue o do AP conectado.
        Serial.printf("Wi-Fi conectado no canal %d para OTA e ESP-NOW.\n", WiFi.channel());
    }

    if (esp_now_init() != ESP_OK) {
        Serial.println("Erro ao iniciar ESP-NOW (Transmissor)");
        return;
    }

    esp_now_register_send_cb(OnDataSent);

    // Configura as informações do par (Carrinho)
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, macDestino, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Falha ao parear com o Carrinho");
        return;
    }
}

void EspNow::sendData(CarData dados) {
    esp_now_send(macDestino, (uint8_t *) &dados, sizeof(dados));
}