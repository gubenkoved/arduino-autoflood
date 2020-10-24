#ifndef INCLUDE_MENU
#define INCLUDE_MENU

enum MenuItemType
{
    SubMenu = 0,
    Command = 1,
    Back = 2
};

class MenuItem
{
private:
    String _name;
public:
    MenuItem(String name);
    const String &GetName() const;
    virtual MenuItemType GetType() const = 0;
};

class GoBackMenuItem : public MenuItem
{
public:
    GoBackMenuItem();
    MenuItemType GetType() const;
};

class SubMenuMenuItem : public MenuItem
{
private:
    MenuItem **_items;
    int _count;

public:
    SubMenuMenuItem(String name, MenuItem *items[], int count);
    int Count() const;
    MenuItem **GetItems() const;
    MenuItemType GetType() const;
    int IndexOf(MenuItem *item) const;
    MenuItem *GetByIndex(int idx) const;
};

class CommandMenuItem : public MenuItem
{
private:
    int _commandId;

public:
    CommandMenuItem(int id, String name);
    int CommandId() const;
    MenuItemType GetType() const;
};

class Menu
{
private:
    SubMenuMenuItem *_root;
    int _level;
    MenuItem **_selection;
    void(*_onCommand)(int);
public:
    Menu(SubMenuMenuItem *rootSubmenu, void(*onCommand)(int) = NULL, int maxLevel = 5);
    ~Menu();
    void Next();
    void Exec();
    int GetLevel();
    MenuItem **GetSelectionStack();
    SubMenuMenuItem *GetRoot();
};
#endif /* INCLUDE_MENU */
