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
}

void SmartButton::setup()
{
    pinMode(_pin, INPUT);
}

void SmartButton::loop()
{
    int curButtonState = digitalRead(_pin);

    if (curButtonState == LOW)
    {
        // catch the moment we release the button
        if (_lastState == HIGH)
        {
            unsigned long duration = millis() - _pressedAt;
            _pressedAt = 0;

            debug(F("Button is released, duration was "));
            debug(duration);
            debugln(F("ms"));

            if (duration >= _longPressThresholdMs)
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
    }
    else // button is pressed
    {
        if (_pressedAt == 0)
        {
            _pressedAt = millis();
            debug(F("Pressed at "));
            debugln(_pressedAt);
        }
    }

    _lastState = curButtonState;
}