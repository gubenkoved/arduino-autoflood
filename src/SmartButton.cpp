#include <Arduino.h>
#include <SmartButton.h>

SmartButton::SmartButton(int pin, void (*onShortPress)(), void (*onLongPress)(), int longPressThresholdMs)
{
    Serial.println(F("SmartButton::ctor"));

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

            Serial.print(F("Button is released, duration was "));
            Serial.print(duration);
            Serial.println(F("ms"));

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
            Serial.print(F("Pressed at "));
            Serial.println(_pressedAt);
        }
    }

    _lastState = curButtonState;
}