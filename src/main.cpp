#include <Arduino.h>
#include <helpers.h>
#include <Menu.h>
#include <SmartButton.h>
#include <MenuRenderer.h>
#include <AutofloodController.h>

// http://paulmurraycbr.github.io/ArduinoTheOOWay.html

Menu *menu = NULL;
MenuRenderer *renderer = NULL;
SmartButton *smartButton = NULL;
AutofloodController *autofloodController = NULL;

unsigned long lastTime;
const int buttonPin = 2;
const int pumpPin = 12;
const int ledPin = 13;

void onButtonShortPress()
{
    menu->Next();
    renderer->Render();
}

void onButtonLongPress()
{
    menu->Exec();
    renderer->Render();
}

// showcase the commands!
void onCommand(int commandId)
{
    debug("EXECUTE COMMAND ID=");
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
        } else
        {
            debugln(F("unable to decrease the duration more!"));
        }
    }
    else if (commandId == 30) // test flood
    {
        // flood in 5 seconds!
        autofloodController->SetNextActivation(5000);
    }
    else if (commandId == 99) // factory reset
    {
        autofloodController->FactoryReset();
    }
}

void onPumpControlMessage(PumpControlMessage message)
{
    debug("PUMP CONTROLL MESSAGE: ");
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
    while (!Serial)
    {
    }

    debugln(F("AUTOFLOOD :: BOOTING UP"));
    debugln("DEBUGGING MODE!");

    pinMode(buttonPin, INPUT);
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

    SubMenuMenuItem *periodMenu = new SubMenuMenuItem("period", periodMenuItems, 7);

    MenuItem *durationMenuItems[] = {
        new CommandMenuItem(10, "+ 0.5s"),
        new CommandMenuItem(11, "- 0.5s"),
        new GoBackMenuItem()};

    SubMenuMenuItem *durationMenu = new SubMenuMenuItem("duration (seconds)", durationMenuItems, 3);

    MenuItem *factoryResetCommand = new CommandMenuItem(99, "factory reset");
    MenuItem *testFloodingCommand = new CommandMenuItem(30, "test flooding");

    MenuItem *rootItems[] =
        {
            periodMenu,
            durationMenu,
            testFloodingCommand,
            factoryResetCommand,
        };

    SubMenuMenuItem *root = new SubMenuMenuItem("root", rootItems, 4);

    menu = new Menu(root, onCommand);

    //renderer = new FullDebugMenuRenderer(menu);
    renderer = new SimpleDebugMenuRenderer(menu);

    smartButton = new SmartButton(buttonPin, onButtonShortPress, onButtonLongPress, LONG_PRESS_THRESHOLD);

    diag();

    renderer->Render();

    autofloodController = new AutofloodController(onPumpControlMessage);

    autofloodController->SetPeriod(10UL);
    autofloodController->SetPumpDuration(1000UL);
    autofloodController->SetNextActivation(10000UL);

    // load the stored settings
    autofloodController->LoadFromMemory();

    lastTime = millis();
}

unsigned long lastStatTime = 0;

void loop()
{
    unsigned long currentTime = millis();
    unsigned long elapsedMs = currentTime - lastTime;

    lastTime = currentTime;

    smartButton->loop();
    autofloodController->HandleElapsed(elapsedMs);

    if (currentTime - lastStatTime > DEBUG_STATE_REPORT_INTERVAL_SECONDS * 1000UL)
    {
        lastStatTime = currentTime;

        // debug stats
        AutofloodState state = autofloodController->GetState();
        unsigned long periodSec = autofloodController->GetPeriodSeconds();
        unsigned long durationMs = autofloodController->GetPumpDurationMs();
        unsigned long nextActivationMs = autofloodController->GetNextActivationMs();

        debug(F("STATE "));
        debug(AutofloodController::StateToString(state));
        debug(F(" PERIOD "));
        AutofloodController::DebugPrintDuration(periodSec * 1000UL);
        debug(F(" DURATION "));
        AutofloodController::DebugPrintDuration(durationMs);
        debug(F(" NEXT ACTIVATION "));
        AutofloodController::DebugPrintDuration(nextActivationMs);
        debugln();
    }
}