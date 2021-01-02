#include <Renderer.h>
#include <helpers.h>

Renderer::Renderer(Menu *menu)
{
    _menu = menu;
}

void FullDebugRenderer::PrintSpaces(int n)
{
    for (int i = 0; i < n; i += 1)
        Serial.print(F(" "));
}

void FullDebugRenderer::RenderImpl(MenuItem *item, int indent)
{
    int type = item->GetType();

    // check if the item is selected
    bool isSelected = false;

    MenuItem **selectionStack = _menu->GetSelectionStack();
    int level = _menu->GetLevel();

    for (int selectionLevel = 1; selectionLevel <= level; selectionLevel += 1)
        if (selectionStack[selectionLevel] == item)
            isSelected = true;

    if (!isSelected)
    {
        PrintSpaces(indent);
    }
    else
    {
        PrintSpaces(indent - 2);
        Serial.print(F("> "));
    }

    // skip root
    if (indent != 0)
        Serial.println(item->GetName());

    if (type == 0) // submenu
    {
        SubMenuMenuItem *submenu = static_cast<SubMenuMenuItem *>(item);

        MenuItem **subintems = submenu->GetItems();
        int count = submenu->Count();

        for (int idx = 0; idx < count; idx += 1)
            RenderImpl(subintems[idx], indent + _indentionStep);
    }
}

FullDebugRenderer::FullDebugRenderer(Menu *menu)
    : Renderer(menu)
{
    debugln(F("FullDebugRenderer::ctor"));
}
FullDebugRenderer::~FullDebugRenderer()
{
    debugln(F("~FullDebugRenderer"));
}

void FullDebugRenderer::RenderMenu()
{
    SubMenuMenuItem *item = _menu->GetRoot();
    RenderImpl(item, 0);
}

// *****************************************************************************

SimpleDebugRenderer::SimpleDebugRenderer(Menu *menu)
    : Renderer(menu)
{
    debugln(F("SimpleDebugRenderer::ctor"));
}

void SimpleDebugRenderer::RenderMenu()
{
    int level = _menu->GetLevel();
    MenuItem **selectionStack = _menu->GetSelectionStack();
    SubMenuMenuItem *container = static_cast<SubMenuMenuItem *>(selectionStack[level - 1]);

    int n = container->Count();
    MenuItem **items = container->GetItems();

    for (int idx = 0; idx < n; idx += 1)
    {
        MenuItem *item = items[idx];

        if (item == selectionStack[level])
            Serial.print(F("> "));
        else
            Serial.print(F("  "));

        Serial.println(item->GetName());
    }
}