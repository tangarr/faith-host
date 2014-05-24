#ifndef BUTTONBOXWIDGET_H
#define BUTTONBOXWIDGET_H

#include "widget.h"
#include "QStringList"

class ButtonBoxWidget : public Widget
{
    QStringList _buttons;
    int _selected;
    int _buttonWidth;
    int _x, _y;
public:
    ButtonBoxWidget(const QStringList &buttons, int buttonWidth);
    int draw(int row, int margin);
    bool keyUpPressed();
    bool keyDownPressed();
    bool keyPressed(int key);
    virtual bool isFocusable() const;
};

#endif // BUTTONBOXWIDGET_H
