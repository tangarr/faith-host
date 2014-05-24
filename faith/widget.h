#ifndef WIDGET_H
#define WIDGET_H

struct _win_st;
class Window;

class Widget
{
private:
    Window* _parent;
    bool    _isFocuable;
    bool    _isFocused;
protected:
    _win_st *parentWnd() const;
public:
    Widget(Window* parent);
    virtual ~Widget();
    Window *parent() const;
    bool isFocusable() const;
    virtual int draw(int row, int margin=1) = 0;
    bool focus(bool focused=true);
    virtual bool keyUpPressed();
    virtual bool keyDownPressed();
    virtual bool keyPressed(int key);
};

#endif // WIDGET_H
