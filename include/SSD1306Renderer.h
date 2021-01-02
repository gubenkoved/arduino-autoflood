#ifndef SSD1306Renderer_H
#define SSD1306Renderer_H

#include <Adafruit_SSD1306.h>

class SSD1306Renderer : public MenuRenderer
{
    private:
        Adafruit_SSD1306 _display;

    public:
        SSD1306Renderer(Menu *menu);
        void Render();
        void Clean();
};

#endif