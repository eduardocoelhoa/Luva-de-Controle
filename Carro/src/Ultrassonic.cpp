#include "Ultrassonic.h"

namespace {
constexpr unsigned long kEchoTimeoutUs = 30000;
constexpr unsigned long kMinSampleIntervalMs = 70;
constexpr float kNoEchoDistanceCm = 999.0f;
}

Ultrassonic::Ultrassonic(int triggerPin, int echoPin) {
    this->triggerPin = triggerPin;
    this->echoPin = echoPin;
    this->sound_speed = 343.0;
}

void Ultrassonic::begin() {
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
    digitalWrite(triggerPin, LOW);
}

void Ultrassonic::pulse() {
    // Envia um pulso de 10 microssegundos para o pino de trigger
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
}

unsigned long Ultrassonic::measureEchoDuration() {
    pulse();
    return pulseIn(echoPin, HIGH, kEchoTimeoutUs);
}

float Ultrassonic::getDistance() {

    // Lê o tempo do pulso de retorno no pino de echo
    unsigned long duration = measureEchoDuration();
    if (duration == 0) {
        return kNoEchoDistanceCm;
    }
    // Calcula a distância com base no tempo e na velocidade do som
    float distance = (duration * sound_speed) / 20000.0; // Dividido por 20000 para converter para cm
    return distance;
}

void Ultrassonic::update() {
    unsigned long now = millis();
    if (now - lastSampleTime < sampleIntervalMs) {
        return;
    }

    lastSampleTime = now;
    lastDistance = getDistance();
}

void Ultrassonic::printDistance() {
    update();

    if (lastDistance >= kNoEchoDistanceCm) {
        Serial.println("Ultrassonic: sem eco (timeout)");
        return;
    }

    Serial.printf("Ultrassonic: %.2f cm\n", lastDistance);
}
