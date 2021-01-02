#ifndef ApplicationStateController_H
#define ApplicationStateController_H

#include <MenuRenderer.h>
#include <Arduino.h>

enum ApplicationState
{
    // user did not interact with the UX for quite some time,
    // user interfaces are off, and interaction causes state transition
    Idle = 0,
    Active = 1,
};

class ApplicationStateController
{
    private:
        ApplicationState _state;
        unsigned long _idleTimeoutMs;
        unsigned long _lastActivityTimer; // time in ms since last user interaction
        MenuRenderer *_menuRenderer;

    public:
        ApplicationStateController(MenuRenderer *menuRenderer, unsigned long idleTimeoutMs = 5000)
        {
            _menuRenderer = menuRenderer;

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
                if (_state == ApplicationState::Active)
                {
                    // state transition to Idle, turn off the display
                    _menuRenderer->Clean();
                }

                _state = ApplicationState::Idle;
            }
            else
            {
                _state = ApplicationState::Active;
            }
        }

        void MarkUserActive()
        {
            _lastActivityTimer = 0;
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

            return "???";
        }
};
#endif /* ApplicationStateController_H */
