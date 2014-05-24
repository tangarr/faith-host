#ifndef WIDGET_H
#define WIDGET_H

class Window;

class Widget
{
private:
    Window* _parent;
    bool    _isFocuable;
    bool    _isFocused;
public:
    Widget(Window* parent);
    virtual ~Widget();
    /**
     * @brief parent
     * @return
     */
    Window *parent() const;
    bool isFocusable() const;
    virtual int draw(int row, int margin=1) = 0;
    bool focus(bool focused=true);
    virtual bool keyUpPressed();
    virtual bool keyDownPressed();
    virtual bool keyPressed(int key);
};

#endif // WIDGET_H
