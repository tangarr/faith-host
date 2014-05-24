#include "buttonboxwidget.h"
#include "window.h"
#include "ncurses.h"

ButtonBoxWidget::ButtonBoxWidget(const QStringList &buttons, int buttonWidth) : Widget()
{
    _buttons = buttons;
    _selected = 0;
    _buttonWidth = buttonWidth;
    _button_pressed = -1;
}

QString centerString(QString str, int width)
{
    if (str.length()>width) return str.left(width);
    else
    {
        QString out="";
        int x=(width-str.length())/2;
        for (int i=0; i<x; i++) out+=" ";
        out+=str;
        for (int i=out.length(); i<width; i++) out+=" ";
        return out;
    }
}

int ButtonBoxWidget::draw(int row, int margin)
{
    int bc = _buttons.count();
    int width = parent()->width()-2*margin;
    int spacing = 1;
    _x = (width+spacing)/(_buttonWidth+spacing);
    int y=0;
    int iter=0;
    int startx = 0;
    if (bc >= _x) startx = margin + (width - (_x*(_buttonWidth+spacing)-spacing))/2;
    else startx = margin + (width - (bc*(_buttonWidth+spacing)-spacing))/2;

    for (int i=0; i<bc; i++)
    {
        QString str = _buttons.at(i);
        bool colored = (isFocused() && i==_selected);
        if (iter>=_x)
        {
            iter=0;
            y++;
        }
        int x = startx+iter*(_buttonWidth+spacing);
        QString but = "|"+centerString(str, _buttonWidth-2)+"|";
        if (colored) parent()->colorOn(Window::ColorWhiteOnBlack);
        mvwprintw(parentWnd(), row+y, x, but.toStdString().c_str());
        if (colored) parent()->colorOff();
        iter++;
    }
    _y = y;
    return row+y;
}

bool ButtonBoxWidget::keyUpPressed()
{
    _button_pressed = -1;
    int y = _selected/_x;
    if (y==0)
    {
        _selected = 0;
        return focusPrev();
    }
    else
    {
        int x = _selected%_x;
        y--;
        int new_select = x+y*_x;
        _selected = new_select;
        return true;
    }
}

bool ButtonBoxWidget::keyDownPressed()
{
    _button_pressed = -1;
    int y = _selected/_x;
    if (y==_y)
    {
        bool tmp = focusNext();
        if (tmp) _selected = 0;
        return tmp;
    }
    else
    {
        int x = _selected%_x;
        y++;
        int new_select = x+y*_x;
        if (new_select>=_buttons.count()-1) new_select = _buttons.count()-1;
        _selected = new_select;
        return true;
    }

}

bool ButtonBoxWidget::keyPressed(int key)
{
    _button_pressed = -1;
    switch (key) {
    case KEY_LEFT:
        if (_selected>0)
        {
            _selected--;
            return true;
        }
        else return false;
    case KEY_RIGHT:
        if (_selected<_buttons.count()-1)
        {
            _selected++;
            return true;
        }
        else return false;
    case 10:
        _button_pressed = _selected;
        return false;
    default:
        return false;
    }

}

bool ButtonBoxWidget::isFocusable() const
{
    return true;
}

int ButtonBoxWidget::buttonPressed()
{
    return _button_pressed;
}
