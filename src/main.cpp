#include <Arduino.h>
#include <helpers.h>
#include <Menu.h>
#include <SmartButton.h>
#include <MenuRenderer.h>
#include <AutofloodController.h>

// #define EXTENDED_DEBUG
// #undef EXTENDED_DEBUG

// http://paulmurraycbr.github.io/ArduinoTheOOWay.html

unsigned long lastTime;
const int buttonPin = 2;
const int pumpPin = 12;

Menu *menu = NULL;
SimpleDebugMenuRenderer *renderer = NULL;
SmartButton *smartButton = NULL;
AutofloodController *autofloodController = NULL;

// showcase the commands!
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

void onCommand(int commandId)
{
    Serial.print("EXECUTE ");
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
        // start in 3 seconds
        autofloodController->SetNextActivation(3000UL);
    }
    else if (commandId == 99) // factory reset
    {
        autofloodController->FactoryReset();
    }
}

GoBackMenuItem backCmd;

CommandMenuItem increasePeriodCmd(1, "+");
CommandMenuItem decreasePeriodCmd(2, "-");
MenuItem *periodMenuItems[] = {&increasePeriodCmd, &decreasePeriodCmd, &backCmd};
SubMenuMenuItem periodMenu("period", periodMenuItems, 3);

CommandMenuItem increaseDuratonCmd(10, "+");
CommandMenuItem decreaseDurationCmd(11, "-");
MenuItem *durationMenuItems[] = {&increaseDuratonCmd, &decreaseDurationCmd, &backCmd};
SubMenuMenuItem durationMenu("duration", durationMenuItems, 3);

CommandMenuItem factoryResetCommand(99, "factory reset");
CommandMenuItem testFloodingCommand(30, "test");
MenuItem *rootItems[] = {&periodMenu, &durationMenu, &factoryResetCommand, &testFloodingCommand};
SubMenuMenuItem root("root", rootItems, 4);


void setup()
{
    Serial.begin(9600);
    while (!Serial)
    {
    } // wait for serial port to connect. Needed for native USB port only

    Serial.println("AUTOFLOOD :: BOOTING UP");
    Serial.flush();

    // pinMode(1, OUTPUT);
    pinMode(2, INPUT);
    //pinMode(3, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);

    menu = new Menu(&root, onCommand);
    renderer = new SimpleDebugMenuRenderer(menu);
    smartButton = new SmartButton(buttonPin, onButtonShortPress, onButtonLongPress);
    autofloodController = new AutofloodController(onPumpControlMessage);

    renderer->Render();

    diag(9);

    diag(10);

    autofloodController->SetPeriod(10UL);
    autofloodController->SetPumpDuration(1000UL);
    autofloodController->SetNextActivation(10000UL);

    diag(11);

    // load the stored settings
    autofloodController->LoadFromMemory();

    lastTime = millis();
}

unsigned long lastStatTime = 0;

void printMeta()
{
    // Serial.print("CURRENT TIME");
    // Serial.print(currentTime);
    // Serial.print(" LAST STATS TIME");
    // Serial.println(lastStatTime);

    // debug stats
    AutofloodState state = autofloodController->GetState();
    unsigned long periodSec = autofloodController->GetPeriodSeconds();
    unsigned long durationMs = autofloodController->GetPumpDurationMs();
    unsigned long nextActivationMs = autofloodController->GetNextActivationMs();

    Serial.print("STATE ");
    Serial.print(AutofloodController::StateToString(state));
    Serial.print(" PERIOD ");
    Serial.print(AutofloodController::PrettyPrintDuration(periodSec * 1000UL));
    Serial.print(" DURATION ");
    Serial.print(AutofloodController::PrettyPrintDuration(durationMs));
    Serial.print(" NEXT ACTIVATION ");
    Serial.println(AutofloodController::PrettyPrintDuration(nextActivationMs));

    diag();
}

void loop()
{
    unsigned long currentTime = millis();
    unsigned long elapsedMs = currentTime - lastTime;

    lastTime = currentTime;

    smartButton->loop();
    autofloodController->HandleElapsed(elapsedMs);

    if (currentTime - lastStatTime > 5000)
    {
        lastStatTime = currentTime;
        printMeta();
    }
}