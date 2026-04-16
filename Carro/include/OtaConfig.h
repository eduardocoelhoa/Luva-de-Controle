#ifndef OTACONFIG_H
#define OTACONFIG_H

#ifndef APP_ENABLE_OTA
#define APP_ENABLE_OTA 0
#endif

namespace OtaConfig {
// Habilitado por ambiente de build (PlatformIO).
constexpr bool kEnable = APP_ENABLE_OTA == 1;

// Credenciais da rede Wi-Fi usada no OTA.
constexpr const char* kWifiSsid = "SEU_WIFI";
constexpr const char* kWifiPassword = "SUA_SENHA";

// Nome que aparece na rede para upload OTA.
constexpr const char* kHostname = "carro-esp32";

// Senha opcional para proteger a atualizacao OTA (pode deixar vazio).
constexpr const char* kPassword = "";

// Timeout maximo para conectar no Wi-Fi antes de seguir sem OTA.
constexpr unsigned long kConnectTimeoutMs = 15000UL;

// Intervalo de atendimento do OTA para evitar custo em todo loop.
constexpr unsigned long kHandleIntervalMs = 15UL;
} // namespace OtaConfig

#endif