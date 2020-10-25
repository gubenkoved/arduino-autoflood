#ifndef INCLUDE_MENURENDERER
#define INCLUDE_MENURENDERER
#include <Arduino.h>
#include <Menu.h>

class MenuRenderer
{
protected:
    Menu *_menu;

public:
    MenuRenderer(Menu *menu);
    virtual void Render() = 0;
};

class FullDebugMenuRenderer : public MenuRenderer
{
private:
    const int _indentionStep = 2;

    void PrintSpaces(int n);
    void RenderImpl(MenuItem *item, int indent);

public:
    FullDebugMenuRenderer(Menu *menu);
    ~FullDebugMenuRenderer();
    void Render();
};

class SimpleDebugMenuRenderer : public MenuRenderer
{
public:
    SimpleDebugMenuRenderer(Menu *menu);
    void Render();
};
#endif /* INCLUDE_MENURENDERER */
