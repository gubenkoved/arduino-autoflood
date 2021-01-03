#include <AutofloodController.h>
#include <EEPROM.h>
#include <helpers.h>

AutofloodController::AutofloodController(PumpControlFn pumpControlFn)
{
    _pumpControlFn = pumpControlFn;
    _periodSeconds = DEFAULT_FLOOD_PERIOD_SEC;
    _pumpDurationMs = DEFAULT_FLOOD_DURATION_MS;
    _nextActivationMs = _periodSeconds * 1000;
    _state = AutofloodState::Waiting;
    _lastAutoSaveTime = millis();
}

void AutofloodController::SetPeriod(unsigned long periodSeconds)
{
    debug(F("Updating pump period "));
    AutofloodController::DebugPrintDuration(periodSeconds * 1000);
    debugln();

    _periodSeconds = periodSeconds;
}

unsigned long AutofloodController::GetPeriodSeconds()
{
    return _periodSeconds;
}

void AutofloodController::SetPumpDuration(unsigned long pumpDurationMs)
{
    debug(F("Updating pump duration "));
    AutofloodController::DebugPrintDuration(pumpDurationMs);
    debugln();

    _pumpDurationMs = pumpDurationMs;
}

unsigned long AutofloodController::GetPumpDurationMs()
{
    return _pumpDurationMs;
}

void AutofloodController::SetNextActivation(unsigned long nextActivationMs)
{
    debug(F("Updating next activation time "));
    AutofloodController::DebugPrintDuration(nextActivationMs);
    debugln();

    _nextActivationMs = nextActivationMs;
}

AutofloodState AutofloodController::GetState()
{
    return _state;
}

unsigned long AutofloodController::GetNextActivationMs()
{
    return _nextActivationMs;
}

void AutofloodController::LoadFromMemory()
{
    debugln(F("Loading data from EEPROM..."));

    AutofloodStoredState state;

    EEPROM.get(EEPROM_START_ADDRESS, state);

    if (state.Version != VERSION)
    {
        debugln(F("Performing factory reset due to version mismatch..."));
        debug(F("CONTROLLER VERSION "));
        debug(VERSION);
        debug(F(" EEPROM STORED VERSION "));
        debugln(state.Version);

        FactoryReset();
        return;
    }

    // move the settings to the controller
    SetNextActivation(state.NextActivationMs);
    SetPumpDuration(state.PumpDurationMs);
    SetPeriod(state.PeriodSeconds);
}

void AutofloodController::SaveToMemory()
{
    debugln(F("Saving settings to EEPROM..."));
    AutofloodStoredState state = AutofloodStoredState();

    state.Version = VERSION;
    state.NextActivationMs = _nextActivationMs;
    state.PeriodSeconds = _periodSeconds;
    state.PumpDurationMs = _pumpDurationMs;

    EEPROM.put(EEPROM_START_ADDRESS, state);
}

void AutofloodController::FactoryReset()
{
    debugln(F("Factory reset..."));

    _periodSeconds = DEFAULT_FLOOD_PERIOD_SEC;
    _pumpDurationMs = DEFAULT_FLOOD_DURATION_MS;
    _nextActivationMs = _periodSeconds * 1000;
    _state = AutofloodState::Waiting;

    // save the settings to the EEPROM
    SaveToMemory();
}

// this method should be called to let the controller perform actions it needs
// caller must provide how many time elapsed to perform time-based activities
void AutofloodController::HandleElapsed(unsigned long elapsedMs)
{
    if (_state == AutofloodState::Waiting)
    {
        if (_nextActivationMs > elapsedMs)
        {
            _nextActivationMs -= elapsedMs;
        }
        else // let's flood!
        {
            _state = AutofloodState::Flooding;
            _pumpControlFn(PumpControlMessage::Open);
            _nextActivationMs = _periodSeconds * 1000UL;
            _floodTimerMs = _pumpDurationMs;
        }
    }
    else // flooding!
    {
        if (_floodTimerMs > elapsedMs)
        {
            _floodTimerMs -= elapsedMs;
        }
        else // shut the pump off!
        {
            _state = AutofloodState::Waiting;
            _pumpControlFn(PumpControlMessage::Close);
        }
    }

    unsigned long currentTime = millis();

    // autosave logic
    if ((currentTime - _lastAutoSaveTime) > AUTOSAVE_INTERVAL_SECONDS * 1000UL)
    {
        debugln(F("Triggering auto save..."));
        SaveToMemory();
        _lastAutoSaveTime = currentTime;
    }
}

// static methods
const char * AutofloodController::StateToString(AutofloodState state)
{
    if (state == AutofloodState::Waiting)
        return "WAITING";

    if (state == AutofloodState::Flooding)
        return "FLOODING";

    return "???";
}

void AutofloodController::DebugPrintDuration(unsigned long durationMs)
{
    const int maxComponentsToShow = 3;
    int shownComponents = 0;
    DurationComponents components = SplitDuration(durationMs);

    if (components.Hours != 0)
    {
        debug(components.Hours);
        debug(F("h "));
        shownComponents += 1;
    }

    if (components.Minutes != 0 && shownComponents < maxComponentsToShow)
    {
        debug(components.Minutes);
        debug(F("m "));
        shownComponents += 1;
    }

    if (components.Seconds != 0 && shownComponents < maxComponentsToShow)
    {
        debug(components.Seconds);
        debug(F("s "));
        shownComponents += 1;
    }

    if (components.Milliseconds != 0 && shownComponents < maxComponentsToShow)
    {
        debug(components.Milliseconds);
        debug(F("ms"));
        shownComponents += 1;
    }
}