#ifndef AUTOFLOODCONTROLLER
#define AUTOFLOODCONTROLLER

#include <Arduino.h>

enum AutofloodState
{
    Waiting = 0,
    Flooding = 1,
};

enum PumpControlMessage
{
    Close = 0,
    Open = 1,
};

// function pointer used to decouple the controller from the way pump
// itself is physically controlled
typedef void (*PumpControlFn)(PumpControlMessage pumpState);

class AutofloodController
{
    int _periodSeconds;    // controls how frequently pump will be activated
    int _pumpDurationMs;   // controls amount of pump working time per activation
    int _nextActivationMs; // next activation timer
    int _floodTimerMs;     // used for "flooding" state, shows how much time is left
    AutofloodState _state;
    PumpControlFn _pumpControlFn;

public:
    AutofloodController(PumpControlFn pumpControlFn)
    {
        _pumpControlFn = pumpControlFn;
        // load the defaults
        _periodSeconds = 24 * 60 * 60;             // every day
        _pumpDurationMs = 2000;                    // two seconds
        _nextActivationMs = _periodSeconds * 1000; // start with the original period to be safe
        _state = AutofloodState::Waiting;
    }

    void SetPeriod(int periodSeconds)
    {
        if (_periodSeconds > 0)
            _periodSeconds = periodSeconds;
    }

    int GetPeriodSeconds()
    {
        return _periodSeconds;
    }

    void SetPumpDuration(int pumpDurationMs)
    {
        if (pumpDurationMs > 0)
            _pumpDurationMs = pumpDurationMs;
    }

    int GetPumpDurationMs()
    {
        return _pumpDurationMs;
    }

    void SetNextActivation(int nextActivationMs)
    {
        if (nextActivationMs > 0)
            _nextActivationMs = nextActivationMs;
    }

    AutofloodState GetState()
    {
        return _state;
    }

    int GetNextActivationMs()
    {
        return _nextActivationMs;
    }

    void LoadFromMemory()
    {
        Serial.println(F("LoadFromMemory -- not implemented!"));
    }

    void SaveToMemory()
    {
        Serial.println(F("SaveToMemory -- not implemented!"));
    }

    // this method should be called to let the controller perform actions it needs
    // caller must provide how many time elapsed to perform time-based activities
    void HandleElapsed(int elapsedMs)
    {
        if (_state == AutofloodState::Waiting)
        {
            _nextActivationMs -= elapsedMs;

            if (_nextActivationMs <= 0)
            {
                // let's flood!
                _state = AutofloodState::Flooding;
                _pumpControlFn(PumpControlMessage::Open);
                _nextActivationMs = _periodSeconds * 1000;
                _floodTimerMs = _pumpDurationMs;
            }
        }
        else // flooding!
        {
            _floodTimerMs -= elapsedMs;

            if (_floodTimerMs <= 0)
            {
                // shut the pump off!
                _state = AutofloodState::Waiting;
                _pumpControlFn(PumpControlMessage::Close);
            }
        }
    }
};
#endif /* AUTOFLOODCONTROLLER */
