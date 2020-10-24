#ifndef INCLUDE_SMARTBUTTON
#define INCLUDE_SMARTBUTTON
#include <Arduino.h>

class SmartButton
{
private:
    int _pin;
    void (*_onShortPress)();
    void (*_onLongPress)();
    int _longPressThresholdMs;
    int _lastState = LOW;
    unsigned long _pressedAt = 0;
public:
    SmartButton(int pin, void (*onShortPress)(), void (*onLongPress)() = NULL, int longPressThresholdMs = 1000);
    void setup();
    void loop();
};
#endif /* INCLUDE_SMARTBUTTON */
