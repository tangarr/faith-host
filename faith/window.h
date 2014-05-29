#ifndef WINDOW_H
#define WINDOW_H

#include <QHash>
#include <QList>
#include <QStringList>

struct _win_st;
struct panel;
class QString;
class Widget;

class Window
{    
public:
    enum Color
    {
        ColorNone,
        ColorSelected,
        ColorMenuInactive,
        ColorMenuActive
    };

private:
    _win_st *_window;
    panel   *_panel;
    QList<Widget*> _widgets;
    Widget *_activeWidget;
    bool focusNextWidget(bool tabPressed=false);
    bool focusPrevWidget();
    static QHash<panel*, Window*> _windows;
    Color _color;
    void addWidget(Widget* widget);
    _win_st *window() const;    
    QString title;
public:
    Window(int height, int width, int row, int column);
    Window(int width, int height);
    virtual ~Window();

    int width() const;
    int height() const;
    void draw();    
    void colorOn(Color c);
    void colorOff();
    virtual bool pressKey(int key);

    static void Initialize();
    static void keyPressedEvent(int key);
    static int screenWidth();
    static int screenHeight();
    static void write(QString msg);
    static void writeLn(QString msg);
    static void refresh();
    friend class Widget;
    static QStringList &splitString(QString msg, int width, int maxHeight=-1);
    static int showMessageBox(QString title, QString message, QStringList buttons);
    static int showComuterLabWindow(const QStringList &laboratories);
    static int getCh();
};

#endif // WINDOW_H
