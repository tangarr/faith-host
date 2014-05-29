#include "widget.h"
#include "window.h"

_win_st *Widget::parentWnd() const
{
    return parent()->window();
}

bool Widget::focusNext()
{
    return parent()->focusNextWidget();
}

bool Widget::focusPrev()
{
    return parent()->focusPrevWidget();
}

Widget::Widget()
{
    _parent = 0;
    _isFocused = false;
}

void Widget::setParent(Window* parent)
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
    return false;
}

bool Widget::isFocused() const
{
    return _isFocused;
}

bool Widget::focus(bool focused)
{
    if (isFocusable())
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
