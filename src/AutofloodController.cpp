#include <AutofloodController.h>
#include <EEPROM.h>
#include <helpers.h>

AutofloodController::AutofloodController(PumpControlFn pumpControlFn)
{
    _pumpControlFn = pumpControlFn;
    // load the defaults
    _periodSeconds = 24UL * 60UL * 60UL;       // every day
    _pumpDurationMs = 2000UL;                  // two seconds
    _nextActivationMs = _periodSeconds * 1000; // start with the original period to be safe
    _state = AutofloodState::Waiting;
    _lastAutoSaveTime = millis();
}

void AutofloodController::SetPeriod(unsigned long periodSeconds)
{
    debug(F("Updating pump period "));
    debugln(AutofloodController::PrettyPrintDuration(periodSeconds * 1000));

    _periodSeconds = periodSeconds;
}

unsigned long AutofloodController::GetPeriodSeconds()
{
    return _periodSeconds;
}

void AutofloodController::SetPumpDuration(unsigned long pumpDurationMs)
{
    debug(F("Updating pump duration "));
    debugln(AutofloodController::PrettyPrintDuration(pumpDurationMs));

    _pumpDurationMs = pumpDurationMs;
}

unsigned long AutofloodController::GetPumpDurationMs()
{
    return _pumpDurationMs;
}

void AutofloodController::SetNextActivation(unsigned long nextActivationMs)
{
    debug(F("Updating next activation time "));
    debugln(AutofloodController::PrettyPrintDuration(nextActivationMs));

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

    _periodSeconds = 60UL * 60UL * 24UL;       // every day
    _pumpDurationMs = 2000UL;                  // two seconds
    _nextActivationMs = _periodSeconds * 1000; // start next day
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

    unsigned long currentTime = millis();

    // autosave logic
    if ((currentTime - _lastAutoSaveTime) > AUTOSAVE_INTERVAL_SECONDS * 1000UL)
    {
        debugln("Triggering auto save...");
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

// TODO: Find a way to switch to c strings
String AutofloodController::PrettyPrintDuration(unsigned long durationMs)
{
    if (durationMs < 1000)
    {
        String result = String(durationMs);
        result += F(" ms");
        return result;
    }

    unsigned long durationSeconds = durationMs / 1000;

    if (durationSeconds < 60)
    {
        String result = String(durationSeconds);
        result += F(" sec");
        return result;
    }

    unsigned long durationMinutes = durationSeconds / 60;

    if (durationMinutes < 60)
    {
        String result = String(durationMinutes);
        result += F(" minutes");
        return result;
    }

    unsigned long durationHours = durationMinutes / 60;
    unsigned long leftOverMinutes = durationMinutes - durationHours * 60UL;

    String result = String(durationHours);
    result += F(" hours ");
    result += leftOverMinutes;
    result += F(" minutes");
    return result;
}