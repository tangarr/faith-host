#include "textwidget.h"
#include "window.h"
#include "ncurses.h"

TextWidget::TextWidget(int height, Algin a) : Widget()
{
    _height = height;
    _algin = a;
}

int TextWidget::draw(int row, int margin)
{    
    int width = parent()->width()-2*margin;
    int rowLeft = parent()->height() - 2 - row;
    if (_height < rowLeft) rowLeft = _height;
    QStringList tmp = Window::splitString(_msg, width, rowLeft);
    _win_st* wnd = parentWnd();
    for (int i=0; i<tmp.count(); i++)
    {
        QString msg="";
        switch (_algin) {
        case AlginLeft:
        {
            mvwprintw(wnd, i+row, margin, tmp.at(i).toStdString().c_str());
            break;
        }
        case AlginRight:
        {
            for (int i=tmp.at(i).length(); i<width; i++) msg+=" ";
            msg+=tmp.at(i);
            mvwprintw(wnd, i+row, margin, msg.toStdString().c_str());
            break;
        }
        case AlginCenter:
        {
            int x=(width-tmp.at(i).length())/2;
            for (int i=0; i<x; i++) msg+=" ";
            msg+=tmp.at(i);
            mvwprintw(wnd, i+row, margin, msg.toStdString().c_str());
            break;
        }
        }
    }
    return row+rowLeft;
}

void TextWidget::setText(QString text)
{
    _msg = text;
}
