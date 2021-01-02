#ifndef SSD1306Renderer_H
#define SSD1306Renderer_H

#include <Adafruit_SSD1306.h>

class SSD1306Renderer : public Renderer
{
    private:
        Adafruit_SSD1306 _display;
        void PrintDuration(unsigned long durationMs);

    public:
        SSD1306Renderer(Menu *menu);
        void RenderMenu();
        void Clean();
        void RenderState(unsigned long nextActivationMs, unsigned long durationMs, unsigned long periodMs);
};

#endif