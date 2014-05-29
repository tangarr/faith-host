#include "menuwidget.h"
#include "window.h"
#include <QStringList>
#include "ncurses.h"

MenuWidget::MenuWidget(int height, const QStringList &items) : Widget()
{
    _height = height;
    _items = items;
    if (items.count() == 0) _selected=-1;
    else _selected = 0;
    _scroll = 0;
    if (_items.count()>_height)
    {
        _scroll_h = _height/_items.count();
    }
    else _scroll_h = 0;
    _enter_pressed = false;
}

bool MenuWidget::isFocusable() const
{
    return true;
}

QString centerString(QString msg, int width, bool selected)
{
    QString out;
    if (selected) out = ">";
    else out = " ";
    width-=2;
    if (msg.length()>=width)
    {
        out+=msg.left(width);
    }
    else
    {
        int blanks = (width-msg.length())/2;
        for (int i=0;i<blanks;i++) out+=" ";
        out+=msg;
        for (int i=out.length(); i<width;i++) out+=" ";
    }
    if (selected) out += "<";
    else out += " ";
    return out;
}

int MenuWidget::draw(int row, int margin)
{
    int width = parent()->width()-2*margin-1;
    for (int i=0; i<_height; i++)
    {
        int index = i+_scroll;
        bool isSelected = (index == _selected);
        if (isFocused())
        {
            if (isSelected)
            {
                parent()->colorOn(Window::ColorSelected);
            }
            else
            {
                parent()->colorOn(Window::ColorMenuActive);
            }
        }
        else parent()->colorOn(Window::ColorMenuInactive);
        QString line="";
        if (index >=0 && index < _items.count())
        {
            line = _items.at(index);
        }
        line = centerString(line, width, isSelected);
        mvwprintw(parentWnd(), row+i, 1+margin, line.toStdString().c_str());
    }
    parent()->colorOff();
    return _height;
}

bool MenuWidget::keyUpPressed()
{
    if (parent())
    {
        if (_selected <= 0)
            return focusPrev();
        else
        {
            _selected--;
            if (_selected < _scroll) _scroll=_selected;
            return true;
        }
    }
    else return false;
}

bool MenuWidget::keyDownPressed()
{
    if (parent())
    {
        if (_selected+1 >= _items.count())
            return focusNext();
        else
        {
            _selected++;
            if (_selected > _scroll+_height) _scroll=_selected-_height;
            return true;
        }
    }
    else return false;
}

bool MenuWidget::keyPressed(int key)
{
    if (key==10)
    {
        _enter_pressed = true;
        return true;
    }
    return false;
}

bool MenuWidget::enterPressed() const
{
    return _enter_pressed;
}

int MenuWidget::selectedIndex() const
{
    return _selected;
}
