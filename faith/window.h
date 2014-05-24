#ifndef WINDOW_H
#define WINDOW_H

#include <QHash>
#include <QList>

struct _win_st;
struct panel;
class QString;
class Widget;

class Window
{    
    _win_st *_window;
    panel   *_panel;
    QList<Widget*> _widgets;
    Widget *_activeWidget;
    bool focusNextWidget(bool tabPressed=false);
    bool focusPrevWidget();
    static QHash<panel*, Window*> _windows;
    void addWidget(Widget* widget);
    _win_st *window() const;
public:
    Window(int width, int height, int row, int column);
    Window(int width, int height);
    ~Window();
    static void Initialize();
    int width() const;
    int height() const;
    void draw();

    virtual void pressKey(int key);
    static void keyPressedEvent(int key);

    static int screenWidth();
    static int screenHeight();
    static void write(QString msg);
    static void writeLn(QString msg);
    static void refresh();

    friend class Widget;
};

#endif // WINDOW_H
