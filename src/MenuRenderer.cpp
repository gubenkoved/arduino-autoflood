#include <MenuRenderer.h>
#include <helpers.h>

MenuRenderer::MenuRenderer(Menu *menu)
{
    _menu = menu;
}

void FullDebugMenuRenderer::PrintSpaces(int n)
{
    for (int i = 0; i < n; i += 1)
        Serial.print(" ");
}

void FullDebugMenuRenderer::RenderImpl(MenuItem *item, int indent)
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
        Serial.print("> ");
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

FullDebugMenuRenderer::FullDebugMenuRenderer(Menu *menu)
    : MenuRenderer(menu)
{
    Serial.println("FullDebugMenuRenderer::ctor");
}
FullDebugMenuRenderer::~FullDebugMenuRenderer()
{
    Serial.println("~FullDebugMenuRenderer");
}

void FullDebugMenuRenderer::Render()
{
    SubMenuMenuItem *item = _menu->GetRoot();
    RenderImpl(item, 0);
}

// *****************************************************************************

SimpleDebugMenuRenderer::SimpleDebugMenuRenderer(Menu *menu)
    : MenuRenderer(menu)
{
    Serial.println("SimpleDebugMenuRenderer::ctor");
}

void SimpleDebugMenuRenderer::Render()
{
    diag(100);

    int level = _menu->GetLevel();
    MenuItem **selectionStack = _menu->GetSelectionStack();
    SubMenuMenuItem *container = static_cast<SubMenuMenuItem *>(selectionStack[level - 1]);

    int n = container->Count();
    MenuItem **items = container->GetItems();

    // Serial.print("LEVEL ");
    // Serial.print(level);
    // Serial.print(" SELECTION STACK PTR ");
    // Serial.print((int)selectionStack);
    // Serial.print(" CONTRAINER PTR ");
    // Serial.print((int)container);
    // Serial.print(" ITEMS PTR ");
    // Serial.print((int)items);
    // Serial.println();

    // for (int idx = 0; idx < n; idx += 1)
    // {
    //     MenuItem *item = items[idx];
    //     MenuItem **item2Ptr = items + idx;

    //     // Serial.print("IDX ");
    //     // Serial.print(idx);
    //     // Serial.print(" PTR ");
    //     // Serial.print((int)item);
    //     // Serial.print(" PTR2 ");
    //     // Serial.print((int)*item2Ptr);
    //     // Serial.print(" PTR2 PTR ");
    //     // Serial.print((int)item2Ptr);
    //     // Serial.print(" SIZEOF PTR ");
    //     // Serial.print(sizeof(MenuItem*));
    //     // Serial.print(" SIZEOF PTR PTR ");
    //     // Serial.print(sizeof(MenuItem**));
    //     // Serial.print(" SIZEOF MENU ITEM ");
    //     // Serial.print(sizeof(MenuItem));
    //     // Serial.println();
    // }

    for (int idx = 0; idx < n; idx += 1)
    {
        MenuItem *item = items[idx];

        // diag(101);
        // Serial.println((int)item);

        if (item == selectionStack[level])
            Serial.print("> ");
        else
            Serial.print("  ");

        const String &menuItemName = item->GetName();
        Serial.println(menuItemName);
        // Serial.println(" MENU ITEM NAME HERE");
    }

    Serial.flush();
}