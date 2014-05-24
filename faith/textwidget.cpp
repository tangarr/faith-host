#include "textwidget.h"
#include "window.h"
#include "ncurses.h"

QStringList &TextWidget::split(QString msg, int width, int maxHeight)
{
    QStringList tmp;
    QStringList *out=new QStringList();
    tmp = msg.split("\n");
    foreach (QString str, tmp) {
        str.trimmed();
        while (str.length()>width)
        {
            str = str.trimmed();
            int index = str.left(width).lastIndexOf(" ");
            if (index==-1)
            {
                out->append(str.left(width));
                str = str.mid(width);
            }
            else
            {
                out->append(str.left(index));
                str = str.mid(index);
            }
            if (out->count()>maxHeight) return *out;
        }
        out->append(str);
        if (out->count()>maxHeight) return *out;
    }
    return *out;
}

TextWidget::TextWidget(Window *parent, int height, Algin a) : Widget(parent)
{
    _height = height;
    _algin = a;
}

int TextWidget::draw(int row, int margin)
{
    int width = parent()->width()-2*margin;
    int rowLeft = parent()->height() - row;
    if (_height < rowLeft) rowLeft = _height;
    QStringList tmp = split(_msg, width, rowLeft);
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
