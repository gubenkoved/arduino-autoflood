#include <Menu.h>
#include <Renderer.h>
#include <SSD1306Renderer.h>
#include <helpers.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)

SSD1306Renderer::SSD1306Renderer(Menu *menu)
    : Renderer(menu)
{
    _display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

    if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    { // Address 0x3C for 128x32
        debugln(F("SSD1306 allocation failed"));
    }

    debugln(F("SSD1306 initialized!"));
    _display.display();
}

void SSD1306Renderer::RenderMenu()
{
    const int lines = 4;

    int level = _menu->GetLevel();

    MenuItem **selectionStack = _menu->GetSelectionStack();
    MenuItem *selected = selectionStack[level];
    SubMenuMenuItem *container = static_cast<SubMenuMenuItem *>(selectionStack[level - 1]);
    MenuItem **items = container->GetItems();

    // setup the display
    _display.clearDisplay();
    _display.setTextSize(1); // default 1:1 scale
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(0, 0);

    // print the layout (with specified limit of lines)
    int curIdx = container->IndexOf(selected);
    int startIdx = max(0, curIdx - lines + 1);

    for (int idx = startIdx; idx < startIdx + lines; idx += 1)
    {
        MenuItem *item = items[idx];
        // MenuItemType itemType = item->GetType();

        if (item == selectionStack[level])
            _display.print(F("> "));
        else
            _display.print(F("  "));


        // display the item name
        _display.print(item->GetName());

        // if (itemType == MenuItemType::SubMenu)
        //     _display.print(F("..."));

        _display.println();
    }

    // show the buffer
    _display.display();
}

void SSD1306Renderer::Clean()
{
    _display.clearDisplay();
    _display.display();
}

void SSD1306Renderer::RenderState(unsigned long nextActivationMs,
                                  unsigned long durationMs,
                                  unsigned long periodMs)
{
    // setup the settings
    _display.clearDisplay();
    _display.setTextSize(1); // default 1:1 scale
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(0, 0);

    // render the data
    _display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    _display.print(F("NEXT"));
    _display.setTextColor(SSD1306_WHITE);
    _display.print(F("     "));
    PrintDuration(nextActivationMs);
    _display.println();

    _display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    _display.print(F("DURATION"));
    _display.setTextColor(SSD1306_WHITE);
    _display.print(F(" "));
    PrintDuration(durationMs);
    _display.println();

    _display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    _display.print(F("PERIOD"));
    _display.setTextColor(SSD1306_WHITE);
    _display.print(F("   "));
    PrintDuration(periodMs);
    _display.println();

    _display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    _display.setTextColor(SSD1306_WHITE);
    _display.println(".. long press to MENU");
    _display.display();
}

void SSD1306Renderer::PrintDuration(unsigned long durationMs)
{
    if (durationMs < 1000UL)
    {
        _display.print(durationMs);
        _display.print(F("ms"));
        return;
    }

    unsigned long durationSeconds = durationMs / 1000UL;

    if (durationSeconds < 60UL)
    {
        unsigned long leftoverMs = durationMs - durationSeconds * 1000UL;

        _display.print(durationSeconds);
        _display.print(F("s"));

        if (leftoverMs != 0)
        {
            _display.print(F(" "));
            _display.print(leftoverMs);
            _display.print(F("ms"));
        }
        return;
    }

    unsigned long durationMinutes = durationSeconds / 60UL;

    if (durationMinutes < 60UL)
    {
        unsigned long leftoverSeconds = durationSeconds - durationMinutes * 60UL;

        _display.print(durationMinutes);
        _display.print(F("m "));
        _display.print(leftoverSeconds);
        _display.print(F("s"));
        return;
    }

    unsigned long durationHours = durationMinutes / 60UL;
    unsigned long leftoverMinutes = durationMinutes - durationHours * 60UL;
    unsigned long leftoverSeconds = durationSeconds - durationHours * 60UL * 60UL - leftoverMinutes * 60UL;

    _display.print(durationHours);
    _display.print(F("h "));

    if (leftoverMinutes != 0)
    {
        _display.print(leftoverMinutes);
        _display.print(F("m "));
    }

    if (leftoverSeconds != 0)
    {
        _display.print(leftoverSeconds);
        _display.print(F("s"));
    }
}