#ifndef INCLUDE_SMARTBUTTON
#define INCLUDE_SMARTBUTTON
#include <Arduino.h>

class SmartButton
{
private:
    int _pin;
    void (*_onShortPress)();
    void (*_onLongPress)();
    unsigned int _longPressThresholdMs;
    unsigned long _pressDurationMs = 0;
public:
    SmartButton(int pin, void (*onShortPress)(), void (*onLongPress)() = NULL, unsigned int longPressThresholdMs = 600U);
    void HandleElapsed(unsigned long elapsedMs);
};
#endif /* INCLUDE_SMARTBUTTON */
