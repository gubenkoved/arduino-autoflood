#ifndef ApplicationStateController_H
#define ApplicationStateController_H

#include <Renderer.h>
#include <Arduino.h>

enum ApplicationState
{
    // user did not interact with the UX for quite some time,
    // user interfaces are off, and interaction causes state transition
    Idle = 0,
    Active = 1,
    Configuration = 2,
};

// TODO: Responsibilities separation between main.cpp and this controller are
//  obscure and intersecting; consider moerging the logic (maybe even move all to main.cpp)
class ApplicationStateController
{
    private:
        ApplicationState _state;
        unsigned long _idleTimeoutMs;
        unsigned long _lastActivityTimer; // time in ms since last user interaction
        Renderer *_renderer;

    public:
        ApplicationStateController(Renderer *menuRenderer, unsigned long idleTimeoutMs = 5000)
        {
            _renderer = menuRenderer;

            // assume active state at initialization
            _state = ApplicationState::Active;
            _lastActivityTimer = 0;
            _idleTimeoutMs = idleTimeoutMs;
        }

        void HandleElapsed(unsigned long elapsedMs)
        {
            _lastActivityTimer += elapsedMs;

            if (_lastActivityTimer > _idleTimeoutMs)
            {
                // state transition to Idle, turn off the display
                if (_state != ApplicationState::Idle)
                    _renderer->Clean();

                _state = ApplicationState::Idle;
            }
        }

        void MarkUserActive()
        {
            if (_state == ApplicationState::Idle)
                _state = ApplicationState::Active;

            _lastActivityTimer = 0;
        }

        void EnterConfigurationMode()
        {
            _lastActivityTimer = 0;
            _state = ApplicationState::Configuration;
        }

        void ExitConfigurationMode()
        {
            _lastActivityTimer = 0;
            _state = ApplicationState::Active;
        }

        ApplicationState GetState()
        {
            return _state;
        }

        static const char * StateToString(ApplicationState state)
        {
            if (state == ApplicationState::Active)
                return "ACTIVE";

            if (state == ApplicationState::Idle)
                return "IDLE";

            if (state == ApplicationState::Configuration)
                return "CONFIG";

            return "???";
        }
};

#endif
