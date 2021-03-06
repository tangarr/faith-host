#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include "widget.h"
#include <QString>
#include <QStringList>

class TextWidget : public Widget
{
public:
    enum Algin {
        AlginLeft,
        AlginRight,
        AlginCenter
    };
private:
    QString _msg;
    int _height;
    Algin _algin;
public:
    TextWidget(int height, Algin a=AlginLeft);
    int draw(int row, int margin);
    void setText(QString text);
};

#endif // TEXTWIDGET_H
