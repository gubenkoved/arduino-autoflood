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

struct AutofloodStoredState
{
    byte Version;
    unsigned long PeriodSeconds;
    unsigned long PumpDurationMs;
    unsigned long NextActivationMs;
};

class AutofloodController
{
    unsigned long _periodSeconds;    // controls how frequently pump will be activated
    unsigned long _pumpDurationMs;   // controls amount of pump working time per activation
    unsigned long _nextActivationMs; // next activation timer
    unsigned long _floodTimerMs;     // used for "flooding" state, shows how much time is left
    AutofloodState _state;
    PumpControlFn _pumpControlFn;
    unsigned long _lastAutoSaveTime;

    const long AUTO_SAVE_PERIOD_SECONDS = 10;
    const int EEPROM_START_ADDRESS = 0; // const for now
    const int VERSION = 1;              // changing version leads to the factory reset!

public:
    AutofloodController(PumpControlFn pumpControlFn);

    void SetPeriod(unsigned long periodSeconds);
    unsigned long GetPeriodSeconds();
    void SetPumpDuration(unsigned long pumpDurationMs);
    unsigned long GetPumpDurationMs();
    void SetNextActivation(unsigned long nextActivationMs);
    AutofloodState GetState();
    unsigned long GetNextActivationMs();
    void LoadFromMemory();
    void SaveToMemory();
    void FactoryReset();

    // this method should be called to let the controller perform actions it needs
    // caller must provide how many time elapsed to perform time-based activities
    void HandleElapsed(unsigned long elapsedMs);

    static const char * StateToString(AutofloodState state);
    static String PrettyPrintDuration(unsigned long durationMs);
};
#endif /* AUTOFLOODCONTROLLER */
