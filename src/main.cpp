#include <Arduino.h>
#include <helpers.h>
#include <Menu.h>
#include <SmartButton.h>
#include <Renderer.h>
#include <AutofloodController.h>
#include <ApplicationStateController.h>
#include <PeriodScheduler.h>
#include <SSD1306Renderer.h>

// http://paulmurraycbr.github.io/ArduinoTheOOWay.html

Menu *menu = NULL;
Renderer *renderer = NULL;
SmartButton *smartButton = NULL;
AutofloodController *autofloodController = NULL;
ApplicationStateController *appStateController = NULL;
PeriodScheduler *stateRefresher = NULL;

unsigned long lastTime;
const int buttonPin = 2;
const int pumpPin = 12;
const int ledPin = 13;

void renderState()
{
    ApplicationState state = appStateController->GetState();

    if (state == ApplicationState::Active)
    {
        unsigned long periodSec = autofloodController->GetPeriodSeconds();
        unsigned long durationMs = autofloodController->GetPumpDurationMs();
        unsigned long nextActivationMs = autofloodController->GetNextActivationMs();
        renderer->RenderState(nextActivationMs, durationMs, periodSec * 1000UL);
    }
}

void onButtonShortPress()
{
    ApplicationState state = appStateController->GetState();
    appStateController->MarkUserActive();

    if (state == ApplicationState::Configuration)
    {
        menu->Next();
        renderer->RenderMenu();
    } else // should be Active state there
    {
        // render the current state
        renderState();
    }
}

void onButtonLongPress()
{
    ApplicationState appState = appStateController->GetState();

    if (appState != ApplicationState::Configuration)
    {
        // enter the configuration state by long press
        appStateController->EnterConfigurationMode();
    } else // ApplicationState::Configuration
    {
        menu->Exec();
    }

    // execution of the command may exit the config mode, so rerender
    // the menu if we still in config mode
    appState = appStateController->GetState();

    if (appState == ApplicationState::Configuration)
        renderer->RenderMenu();

    appStateController->MarkUserActive();
}

// showcase the commands!
void onCommand(int commandId)
{
    debug(F("EXECUTE COMMAND ID="));
    debugln(commandId);

    unsigned long currentPeriodSeconds = autofloodController->GetPeriodSeconds();
    unsigned long currentDurationMs = autofloodController->GetPumpDurationMs();

    if (commandId == 1) // period: +1h
    {
        autofloodController->SetPeriod(currentPeriodSeconds + 60 * 60);
    }
    else if (commandId == 2) // period: -1h
    {
        autofloodController->SetPeriod(currentPeriodSeconds - 60 * 60);
    }
    else if (commandId == 3) // period: +5m
    {
        autofloodController->SetPeriod(currentPeriodSeconds + 60 * 5);
    }
    else if (commandId == 4) // period: -5m
    {
        autofloodController->SetPeriod(currentPeriodSeconds - 60 * 5);
    }
    else if (commandId == 5) // period x2
    {
        autofloodController->SetPeriod(currentPeriodSeconds * 2);
    }
    else if (commandId == 6) // period 1/2
    {
        autofloodController->SetPeriod(currentPeriodSeconds / 2);
    }
    else if (commandId == 10) // increase pump duration
    {
        autofloodController->SetPumpDuration(currentDurationMs + 500);
    }
    else if (commandId == 11) // decrease pump duration
    {
        // safety measure, prevent the overflow to high values
        if (currentDurationMs > 500)
        {
            autofloodController->SetPumpDuration(currentDurationMs - 500);
        }
        else
        {
            debugln(F("unable to decrease the duration more!"));
        }
    }
    else if (commandId == 30) // test flood
    {
        // flood in 5 seconds!
        autofloodController->SetNextActivation(5000);
    }
    else if (commandId == 40) // reset timer
    {
        autofloodController->SetNextActivation(currentPeriodSeconds * 1000UL);
    }
    else if (commandId == 50) // exit
    {
        appStateController->ExitConfigurationMode();
        renderState();
    }
    else if (commandId == 99) // factory reset
    {
        autofloodController->FactoryReset();
    }
}

void onPumpControlMessage(PumpControlMessage message)
{
    debug(F("PUMP CONTROLL MESSAGE: "));
    debugln(message);

    if (message == PumpControlMessage::Open)
    {
        digitalWrite(pumpPin, HIGH);
        digitalWrite(ledPin, HIGH);
    }
    else if (message == PumpControlMessage::Close)
    {
        digitalWrite(pumpPin, LOW);
        digitalWrite(ledPin, LOW);
    }
}

void setup()
{
    Serial.begin(9600);

    // wait for serial port to connect. Needed for native USB port only
    while (!Serial) { }

    debugln(F("AUTOFLOOD :: BOOTING UP"));
    debugln(F("DEBUGGING MODE!"));

    pinMode(pumpPin, OUTPUT);
    pinMode(ledPin, OUTPUT);

    MenuItem *periodMenuItems[] = {
        new CommandMenuItem(1, "+ 1h"),
        new CommandMenuItem(2, "- 1h"),
        new CommandMenuItem(3, "+ 5m"),
        new CommandMenuItem(4, "- 5m"),
        new CommandMenuItem(5, "x2"),
        new CommandMenuItem(6, "1/2"),
        new GoBackMenuItem()};

    SubMenuMenuItem *periodMenu = SubMenuMenuItem::Create("period", periodMenuItems);

    MenuItem *durationMenuItems[] = {
        new CommandMenuItem(10, "+ 0.5s"),
        new CommandMenuItem(11, "- 0.5s"),
        new GoBackMenuItem()};

    SubMenuMenuItem *durationMenu = SubMenuMenuItem::Create("duration", durationMenuItems);

    MenuItem *floodNowCommand = new CommandMenuItem(30, "flood now");
    MenuItem *resetTimerCommand = new CommandMenuItem(40, "reset timer");
    MenuItem *factoryResetCommand = new CommandMenuItem(99, "factory reset");
    MenuItem *exitCommand = new CommandMenuItem(50, "<exit>");

    MenuItem *rootItems[] =
        {
            periodMenu,
            durationMenu,
            floodNowCommand,
            resetTimerCommand,
            factoryResetCommand,
            exitCommand,
        };

    SubMenuMenuItem *root = SubMenuMenuItem::Create("root", rootItems);

    menu = new Menu(root, onCommand);

    // renderer = new FullDebugRenderer(menu);
    // renderer = new SimpleDebugRenderer(menu);
    renderer = new SSD1306Renderer(menu);

    // note that SmartButton will automatically adjust the PIN mode to INPUT
    smartButton = new SmartButton(buttonPin, onButtonShortPress, onButtonLongPress, LONG_PRESS_THRESHOLD);

    diag();

    autofloodController = new AutofloodController(onPumpControlMessage);

    // load the stored settings
    autofloodController->LoadFromMemory();

    appStateController = new ApplicationStateController(renderer, IDLE_TIMEOUT_MS);

    lastTime = millis();

    stateRefresher = new PeriodScheduler(renderState, STATE_REFRESH_INTERVAL_MS, true);
}

unsigned long lastStatTime = 0;

void loop()
{
    unsigned long currentTime = millis();
    unsigned long elapsedMs = currentTime - lastTime;

    // note that even that we operate on small (in ms) integer durations
    // and Flood Controller uses that to count the time it still provides very
    // good accuracy (in my tests the skew was in order of seconds for the whole
    // day) because the millis() func seem to rely on a higher resolution timer
    // and so the diff between real elapsed time and out integer approximation
    // do not add up (!), but rather compensate each other
    if (elapsedMs == 0)
    {
        delay(1);
        return;
    }

    lastTime = currentTime;

    // let dependencies handle the elapsed time
    smartButton->HandleElapsed(elapsedMs);
    autofloodController->HandleElapsed(elapsedMs);
    appStateController->HandleElapsed(elapsedMs);
    stateRefresher->HandleElapsed(elapsedMs);

    if (currentTime - lastStatTime >= DEBUG_STATE_REPORT_INTERVAL_MS)
    {
        lastStatTime = currentTime;

        // debug stats
        AutofloodState state = autofloodController->GetState();
        unsigned long periodSec = autofloodController->GetPeriodSeconds();
        unsigned long durationMs = autofloodController->GetPumpDurationMs();
        unsigned long nextActivationMs = autofloodController->GetNextActivationMs();

        ApplicationState appState = appStateController->GetState();

        debug(F("APP STATE "));
        debug(ApplicationStateController::StateToString(appState));
        debug(F(" FLOOD STATE "));
        debug(AutofloodController::StateToString(state));
        debug(F(" PERIOD "));
        AutofloodController::DebugPrintDuration(periodSec * 1000UL);
        debug(F("DURATION "));
        AutofloodController::DebugPrintDuration(durationMs);
        debug(F("NEXT ACTIVATION "));
        AutofloodController::DebugPrintDuration(nextActivationMs);
        debugln();
    }
}