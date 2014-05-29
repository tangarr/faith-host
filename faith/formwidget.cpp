#include "formwidget.h"
#include "ncurses.h"
#include "window.h"

FormWidget::FormWidget(QString name, int nameWidth, QString pattern)
{
    _name = name.left(nameWidth);
    _nameWidth = nameWidth;
    _regexp = QRegExp(pattern);
    _value="";
    _readOnly = false;
}

bool FormWidget::isFocusable() const
{
    return !_readOnly;
}

int FormWidget::draw(int row, int margin)
{
    int width = parent()->width() - 3*margin - _nameWidth-2;
    mvwprintw(parentWnd(), row, margin+1, _name.toStdString().c_str());

    if (_regexp.exactMatch(_value))
    {
        if (isFocused()) parent()->colorOn(Window::ColorSelected);
    }
    else
    {
        if (isFocused()) parent()->colorOn(Window::ColorErrorField);
        else parent()->colorOn(Window::ColorErrorText);
    }

    QString out;
    if (_value.length()>=width-1) out=_value.right(width-1);
    else
    {
        out=_value;
        for (int i=out.length();i<width;i++) out+="_";
    }
    mvwprintw(parentWnd(), row, 1+margin*2+_nameWidth, out.toStdString().c_str());
    if (isFocused())
    {
        int pos = _value.length();
        if (pos>width-1) pos=width-1;
        mvwaddch(parentWnd(), row, 1+margin*2+_nameWidth+pos, '|');
    }
    parent()->colorOff();
    return row+1;
}

bool FormWidget::keyPressed(int key)
{
    if (key==KEY_BACKSPACE)
    {
        if (_value.length()>0)
        {
            _value = _value.left(_value.length()-1);
            return true;
        }
        else return false;
    }
    else if (isprint(key))
    {
        QChar c(key);
        _value=_value+c;
        return true;
    }
    else return false;
}

void FormWidget::setValue(const QString &value)
{
    _value = value;
}

QString FormWidget::value() const
{
    return _value;
}

void FormWidget::setReadOnly(bool readOnly)
{
    _readOnly = readOnly;
}

bool FormWidget::isValid() const
{
    return _regexp.exactMatch(_value);
}
