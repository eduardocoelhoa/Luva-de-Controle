#ifndef OTASERVICE_H
#define OTASERVICE_H

class OtaService {
public:
    void begin();
    void update();

    bool isResolved() const;
    bool isReady() const;

private:
    enum class State {
        Disabled,
        Connecting,
        Ready,
        Failed
    };

    State state = State::Disabled;
    unsigned long connectStartMs = 0;
    unsigned long lastHandleMs = 0;

    void startArduinoOta();
};

#endif
