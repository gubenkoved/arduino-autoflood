#include <Arduino.h>
#include <Menu.h>
#include <helpers.h>

MenuItem::MenuItem(const char *name)
{
    _name = name;
}

const char *MenuItem::GetName() const
{
    return _name;
}

// *****************************************************************************

GoBackMenuItem::GoBackMenuItem()
    : MenuItem("<back>")
{
}

MenuItemType GoBackMenuItem::GetType() const
{
    return MenuItemType::Back;
}

// *****************************************************************************

SubMenuMenuItem::SubMenuMenuItem(const char *name, MenuItem *items[], int count)
    : MenuItem(name)
{
    _items = items;
    _count = count;
}

int SubMenuMenuItem::Count() const
{
    return _count;
}

MenuItem **SubMenuMenuItem::GetItems() const
{
    return _items;
}

MenuItemType SubMenuMenuItem::GetType() const
{
    return MenuItemType::SubMenu;
}

int SubMenuMenuItem::IndexOf(MenuItem *item) const
{
    for (int idx = 0; idx < _count; idx += 1)
        if (item == _items[idx])
            return idx;

    return -1;
}

MenuItem *SubMenuMenuItem::GetByIndex(int idx) const
{
    idx = idx % _count;
    return _items[idx];
}

// *****************************************************************************

CommandMenuItem::CommandMenuItem(int id, const char *name)
    : MenuItem(name)
{
    _commandId = id;
}

int CommandMenuItem::CommandId() const
{
    return _commandId;
}

MenuItemType CommandMenuItem::GetType() const
{
    return MenuItemType::Command;
}

// *****************************************************************************

Menu::Menu(SubMenuMenuItem *rootSubmenu, void(*onCommand)(int), int maxLevel)
{
    debugln(F("initializing the menu..."));

    _onCommand = onCommand;
    _root = rootSubmenu;
    _level = 1;
    _selection = new MenuItem *[maxLevel];
    _selection[0] = rootSubmenu;

    // when we start select the first element
    _selection[1] = rootSubmenu->GetItems()[0];
}

Menu::~Menu()
{
    debugln(F("destructing the menu"));
}

void Menu::Next()
{
    MenuItem *selected = _selection[_level];
    SubMenuMenuItem *container = static_cast<SubMenuMenuItem *>(_selection[_level - 1]);

    int n = container->Count();
    int curIdx = container->IndexOf(selected);
    int nextIdx = (curIdx + 1) % n;

    // debug("NEXT IDX: ");
    // debugln(nextIdx);

    _selection[_level] = container->GetByIndex(nextIdx);

    // debug("SELECTION: ");
    // debugln(_selection[_level]->GetName());
}

void Menu::Exec()
{
    // exec the current command OR go to sub menu if the it's selected
    MenuItem *selected = _selection[_level];
    int selectedType = selected->GetType();

    if (selectedType == 0)
    {
        debugln(F("GO TO SUBMENU"));

        SubMenuMenuItem *nextMenu = static_cast<SubMenuMenuItem *>(selected);
        _level = _level + 1;
        _selection[_level] = nextMenu->GetItems()[0];
    }
    else if (selectedType == 2)
    {
        debugln(F("GOING BACK"));
        _level = _level - 1;
    }
    else if (selectedType == 1)
    {
        CommandMenuItem *commandItem = static_cast<CommandMenuItem *>(selected);

        if (_onCommand != NULL)
        {
            int commandId = commandItem->CommandId();
            _onCommand(commandId);
        } else
        {
            debug(F("COMMAND HANDLER NOT SPECIFIED"));
        }
    }
}

int Menu::GetLevel()
{
    return _level;
}

MenuItem **Menu::GetSelectionStack()
{
    return _selection;
}

SubMenuMenuItem *Menu::GetRoot()
{
    return _root;
}