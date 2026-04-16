#ifndef ULTRASSONIC_H
#define ULTRASSONIC_H

#include <Arduino.h>

class Ultrassonic {

  private:
    int triggerPin;
    int echoPin;
    double sound_speed;
    unsigned long sampleIntervalMs;
    unsigned long lastSampleTime;
    float lastDistance;

    void pulse();
    unsigned long measureEchoDuration();

  public:
    Ultrassonic(int triggerPin, int echoPin);
    void begin();
    void update();
    void printDistance();
    float getDistance();
};

#endif