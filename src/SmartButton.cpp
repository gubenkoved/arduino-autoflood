#include <Arduino.h>
#include <SmartButton.h>
#include <helpers.h>

SmartButton::SmartButton(int pin, void (*onShortPress)(), void (*onLongPress)(), unsigned int longPressThresholdMs)
{
    debugln(F("SmartButton::ctor"));

    _pin = pin;
    _onShortPress = onShortPress;
    _onLongPress = onLongPress;
    _longPressThresholdMs = longPressThresholdMs;

    // adjust the PIN mode
    pinMode(_pin, INPUT);
}

void SmartButton::HandleElapsed(unsigned long elapsedMs)
{
    int state = digitalRead(_pin);

    if (state == LOW)
    {
        if (_pressDurationMs != 0)
        {
            debug(F("Button is released, duration was "));
            debug(_pressDurationMs);
            debugln(F("ms"));

            if (_pressDurationMs >= _longPressThresholdMs)
            {
                if (_onLongPress != NULL)
                    _onLongPress();
            }
            else
            {
                if (_onShortPress != NULL)
                    _onShortPress();
            }
        }

        _pressDurationMs = 0;
    }
    else // state == HIGH button is pressed
    {
        if (_pressDurationMs == 0)
            debugln(F("Button pressed!"));

        _pressDurationMs += elapsedMs;
    }
}