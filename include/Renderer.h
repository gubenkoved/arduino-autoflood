#ifndef RENDERER_H
#define RENDERER_H
#include <Arduino.h>
#include <Menu.h>

class Renderer
{
protected:
    Menu *_menu;

public:
    Renderer(Menu *menu);
    virtual void RenderMenu() = 0;
    virtual void RenderState(unsigned long nextActivationMs, unsigned long durationMs, unsigned long periodMs)
    {
        // no-op by default
    }
    virtual void Clean()
    {
        // no-op by default
    }
};

// TODO: Move debug state rendering into the Renderers from the main.cpp
class FullDebugRenderer : public Renderer
{
private:
    const int _indentionStep = 2;

    void PrintSpaces(int n);
    void RenderImpl(MenuItem *item, int indent);
public:
    FullDebugRenderer(Menu *menu);
    ~FullDebugRenderer();
    void RenderMenu();
};

class SimpleDebugRenderer : public Renderer
{
public:
    SimpleDebugRenderer(Menu *menu);
    void RenderMenu();
};

#endif
