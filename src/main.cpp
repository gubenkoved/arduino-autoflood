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
const int buttonPin = 2;
const int pumpPin = 12;

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

    if (commandId == 1) // increase period
    {
        int currentPeriod = autofloodController->GetPeriodSeconds();
        autofloodController->SetPeriod(currentPeriod + 1);
    }
    else if (commandId == 2) // descrease period
    {
        int currentPeriod = autofloodController->GetPeriodSeconds();
        autofloodController->SetPeriod(currentPeriod - 1);
    }
    else if (commandId == 10) // increase pump duration
    {
        int currentDurationMs = autofloodController->GetPumpDurationMs();
        autofloodController->SetPumpDuration(currentDurationMs + 500);
    }
    else if (commandId == 11) // decrease pump duration
    {
        int currentDurationMs = autofloodController->GetPumpDurationMs();
        autofloodController->SetPumpDuration(currentDurationMs - 500);
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

    // pinMode(1, OUTPUT);
    pinMode(2, INPUT);
    //pinMode(3, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);

    MenuItem *periodMenuItems[] = {
        new CommandMenuItem(1, "+"),
        new CommandMenuItem(2, "-"),
        new GoBackMenuItem()};

    SubMenuMenuItem *periodMenu = new SubMenuMenuItem("period (hours)", periodMenuItems, 3);

    MenuItem *durationMenuItems[] = {
        new CommandMenuItem(10, "+"),
        new CommandMenuItem(20, "-"),
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

    smartButton = new SmartButton(buttonPin, onButtonShortPress, onButtonLongPress);

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

    if (currentTime - lastStatTime > 1000)
    {
        // Serial.print("CURRENT TIME");
        // Serial.print(currentTime);
        // Serial.print(" LAST STATS TIME");
        // Serial.println(lastStatTime);

        lastStatTime = currentTime;

        // debug stats
        AutofloodState state = autofloodController->GetState();
        unsigned long periodSec = autofloodController->GetPeriodSeconds();
        unsigned long durationMs = autofloodController->GetPumpDurationMs();
        unsigned long nextActivationMs = autofloodController->GetNextActivationMs();

        Serial.print(F("STATE "));
        Serial.print(AutofloodController::StateToString(state));
        Serial.print(F(" PERIOD "));
        Serial.print(AutofloodController::PrettyPrintDuration(periodSec * 1000UL));
        Serial.print(F(" DURATION "));
        Serial.print(AutofloodController::PrettyPrintDuration(durationMs));
        Serial.print(F(" NEXT ACTIVATION "));
        Serial.println(AutofloodController::PrettyPrintDuration(nextActivationMs));
    }
}