#include <Arduino.h>
#include <helpers.h>
#include <Menu.h>
#include <SmartButton.h>
#include <MenuRenderer.h>

int outputStates[14];

// http://paulmurraycbr.github.io/ArduinoTheOOWay.html

Menu * menu = NULL;
MenuRenderer * renderer = NULL;
SmartButton * smartButton = NULL;

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
    if (commandId == 1 || commandId == 2 || commandId == 3 || commandId == 13)
    {
        int pinNumber = commandId;
        outputStates[pinNumber] = outputStates[pinNumber] == LOW ? HIGH : LOW;
        digitalWrite(pinNumber, outputStates[pinNumber]);
    } else
    {
        Serial.print("EXECUTE COMMAND STUB, ID=");
        Serial.println(commandId);
    }
}

void setup()
{
    Serial.begin(9600);
    while (!Serial) {} // wait for serial port to connect. Needed for native USB port only

    Serial.println(F("AUTOFLOOD :: BOOTING UP"));

    pinMode(1, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(13, OUTPUT);

    MenuItem *items[] = {
        new CommandMenuItem(1, F("switch D1")),
        new CommandMenuItem(2, F("switch D2")),
        new CommandMenuItem(3, F("switch D3")),
        new CommandMenuItem(13, F("switch D13")),
        new GoBackMenuItem()};

    SubMenuMenuItem *submenu = new SubMenuMenuItem(F("switch outputs"), items, 5);

    MenuItem *items2[] = {
        new CommandMenuItem(10, F("command-10")),
        new CommandMenuItem(20, F("command-20")),
        new GoBackMenuItem()
    };

    SubMenuMenuItem *submenu2 = new SubMenuMenuItem(F("second-submenu"), items2, 3);

    MenuItem * rootCommand = new CommandMenuItem(0, F("root-command-1"));

    MenuItem *rootItems[] = {submenu, submenu2, rootCommand};

    SubMenuMenuItem *root = new SubMenuMenuItem(F("root"), rootItems, 3);

    menu = new Menu(root, onCommand);

    //renderer = new FullDebugMenuRenderer(menu);
    renderer = new SimpleDebugMenuRenderer(menu);

    smartButton = new SmartButton(5, onButtonShortPress, onButtonLongPress);

    diag();

    renderer->Render();
}

void loop()
{
    smartButton->loop();
}