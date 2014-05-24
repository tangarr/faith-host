#include "widget.h"
#include "window.h"

_win_st *Widget::parentWnd() const
{
    return parent()->window();
}

Widget::Widget(Window *parent)
{
    _parent = parent;
}

Widget::~Widget()
{
    _parent = nullptr;
}

Window *Widget::parent() const
{
    return _parent;
}

bool Widget::isFocusable() const
{
    return _isFocuable;
}

bool Widget::focus(bool focused)
{
    if (_isFocuable)
    {
        _isFocused = focused;
        return true;
    }
    else return false;
}

bool Widget::keyUpPressed()
{
    return _parent->focusPrevWidget();
}

bool Widget::keyDownPressed()
{
    return _parent->focusNextWidget();
}

bool Widget::keyPressed(int /*key*/)
{
    return false;
}
