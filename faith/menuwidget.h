#ifndef MENUWIDGET_H
#define MENUWIDGET_H

#include "widget.h"
#include <QStringList>


class MenuWidget : public Widget
{
    int _height;
    int _scroll;
    int _scroll_h;
    int _selected;
    QStringList _items;
    bool _enter_pressed;
public:
    MenuWidget(int height, const QStringList& items);
    bool isFocusable() const;
    int draw(int row, int margin);
    bool keyUpPressed();
    bool keyDownPressed();
    bool keyPressed(int key);
    bool enterPressed() const;
    int selectedIndex() const;
};

#endif // MENUWIDGET_H
