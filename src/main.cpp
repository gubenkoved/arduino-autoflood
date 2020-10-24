#include <Arduino.h>
#include <helpers.h>
#include <Menu.h>
#include <SmartButton.h>
#include <MenuRenderer.h>
#include <AutofloodController.h>

int outputStates[14];

// http://paulmurraycbr.github.io/ArduinoTheOOWay.html

Menu *menu = NULL;
MenuRenderer *renderer = NULL;
SmartButton *smartButton = NULL;
AutofloodController *autofloodController = NULL;

unsigned long lastTime;
const int pumpPin = 2;

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
    Serial.print("EXECUTE COMMAND ID=");
    Serial.println(commandId);

    if (commandId == 1)
    {
        int currentPeriod = autofloodController->GetPeriodSeconds();
        autofloodController->SetPeriod(currentPeriod + 1);
    }
    else if (commandId == 2)
    {
        int currentPeriod = autofloodController->GetPeriodSeconds();
        autofloodController->SetPeriod(currentPeriod - 1);
    }
    else if (commandId == 10)
    {
        int currentDurationMs = autofloodController->GetPumpDurationMs();
        autofloodController->SetPumpDuration(currentDurationMs + 500);
    }
    else if (commandId == 11)
    {
        int currentDurationMs = autofloodController->GetPumpDurationMs();
        autofloodController->SetPumpDuration(currentDurationMs - 500);
    }
}

void onPumpControlMessage(PumpControlMessage message)
{
    Serial.print("PUMP CONTROLL MESSAGE: ");
    Serial.println(message);

    if (message == PumpControlMessage::Open)
    {
        digitalWrite(pumpPin, HIGH);
    }
    else if (message == PumpControlMessage::Close)
    {
        digitalWrite(pumpPin, LOW);
    }
}

void setup()
{
    Serial.begin(9600);
    while (!Serial)
    {
    } // wait for serial port to connect. Needed for native USB port only

    Serial.println(F("AUTOFLOOD :: BOOTING UP"));

    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(13, OUTPUT);

    MenuItem *periodMenuItems[] = {
        new CommandMenuItem(1, F("+")),
        new CommandMenuItem(2, F("-")),
        new GoBackMenuItem()};

    SubMenuMenuItem *periodMenu = new SubMenuMenuItem(F("period (hours)"), periodMenuItems, 3);

    MenuItem *durationMenuItems[] = {
        new CommandMenuItem(10, F("+")),
        new CommandMenuItem(20, F("-")),
        new GoBackMenuItem()};

    SubMenuMenuItem *durationMenu = new SubMenuMenuItem(F("duration (seconds)"), durationMenuItems, 3);

    MenuItem *factoryResetCommand = new CommandMenuItem(0, F("factory reset"));

    MenuItem *rootItems[] = {periodMenu, durationMenu, factoryResetCommand};

    SubMenuMenuItem *root = new SubMenuMenuItem(F("root"), rootItems, 3);

    menu = new Menu(root, onCommand);

    //renderer = new FullDebugMenuRenderer(menu);
    renderer = new SimpleDebugMenuRenderer(menu);

    smartButton = new SmartButton(5, onButtonShortPress, onButtonLongPress);

    diag();

    renderer->Render();

    autofloodController = new AutofloodController(onPumpControlMessage);

    autofloodController->SetPeriod(10);
    autofloodController->SetPumpDuration(1000);
    autofloodController->SetNextActivation(10000);

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

    
    if (currentTime - lastStatTime > 1000)
    {
        // Serial.print("CURRENT TIME");
        // Serial.print(currentTime);
        // Serial.print(" LAST STATS TIME");
        // Serial.println(lastStatTime);

        lastStatTime = currentTime;

        // debug stats
        AutofloodState state = autofloodController->GetState();
        int periodSec = autofloodController->GetPeriodSeconds();
        int durationMs = autofloodController->GetPumpDurationMs();
        int nextActivationMs = autofloodController->GetNextActivationMs();

        Serial.print(F("STATE "));
        Serial.print(state);
        Serial.print(F(" PERIOD "));
        Serial.print(periodSec);
        Serial.print(F(" DURATION "));
        Serial.print(durationMs);
        Serial.print(F(" NEXT ACTIVATION "));
        Serial.println(nextActivationMs);
    }
}