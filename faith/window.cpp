#include "widget.h"
#include "window.h"
#include "ncurses.h"
#include "panel.h"
#include <QString>
#include <QStringList>

QHash<panel*, Window*> Window::_windows;

void Window::Initialize()
{
    initscr();
    keypad(stdscr, true);
    noecho();
    raw();
    //curs_set(0);
    start_color();
}

bool Window::focusNextWidget(bool tabPressed)
{
    if (_activeWidget==nullptr) return false;
    if (_widgets.count()==0)
    {
        return false;
    }
    else
    {
        int index = _widgets.indexOf(_activeWidget);
        bool val = false;
        for (int i=index; i<_widgets.count(); i++)
        {
            if (_widgets.at(i)->isFocusable())
            {
                _activeWidget->focus(false);
                _activeWidget=_widgets.at(i);
                _activeWidget->focus();
                val = true;
                break;
            }
        }
        if (tabPressed)
        {
            for (int i=0; i<index-1; i++)
            {
                if (_widgets.at(i)->isFocusable())
                {
                    _activeWidget->focus(false);
                    _activeWidget=_widgets.at(i);
                    _activeWidget->focus();
                    val = true;
                    break;
                }
            }
        }
        return val;
    }
}

bool Window::focusPrevWidget()
{
    if (_activeWidget==nullptr) return false;
    if (_widgets.count()==0)
    {
        return false;
    }
    else
    {
        int index = _widgets.indexOf(_activeWidget);
        bool val = false;
        for (int i=index-1; i>=0; i--)
        {
            if (_widgets.at(i)->isFocusable())
            {
                _activeWidget->focus(false);
                _activeWidget=_widgets.at(i);
                _activeWidget->focus();
                val = true;
                break;
            }
        }
        return val;
    }
}

void Window::addWidget(Widget *widget)
{
    _widgets.append(widget);
    if (!_activeWidget && widget->isFocusable())
    {
        _activeWidget = widget;
    }
}

_win_st *Window::window() const
{
    return _window;
}

Window::Window(int width, int height, int row, int column)
{
    _window = newwin(width, height, row, column);
    box(_window, 0, 0);
    _panel = new_panel(_window);
    _windows.insert(_panel, this);
}

Window::Window(int width, int height) : Window(width, height, (screenWidth()-width)/2, (screenHeight()-height)/2)
{
}

Window::~Window()
{
    if (_window)
    {
        delwin(_window);
        _window = nullptr;
    }
    if (_panel)
    {
        _windows.remove(_panel);
        del_panel(_panel);
        _panel = nullptr;
    }
    foreach (Widget* w, _widgets) {
        delete w;
    }
    _widgets.clear();
}

int Window::width() const
{
    if (_window) return getmaxx(_window);
    else return -1;

}

int Window::height() const
{
    if (_window) return getmaxy(_window);
    else return -1;
}

void Window::draw()
{
    int row = 0;
    foreach (Widget* w, _widgets) {
        row = w->draw(row, 1);
        if (row>height()) break;
    }
    wrefresh(_window);
}

void Window::pressKey(int key)
{
    switch (key) {
    case KEY_UP:
        if (_activeWidget) _activeWidget->keyUpPressed();
        break;
    case KEY_DOWN:
        if (_activeWidget) _activeWidget->keyDownPressed();
        break;
    case KEY_BTAB:
        focusNextWidget(true);
        break;
    default:
        if (_activeWidget) _activeWidget->keyPressed(key);
        break;
    }

}

void Window::keyPressedEvent(int key)
{
    panel* top = panel_below(0);
    if (top)
    {
        Window* wnd = _windows.value(top);
        wnd->pressKey(key);
    }
}

int Window::screenWidth()
{
    return getmaxx(stdscr);
}

int Window::screenHeight()
{
    return getmaxy(stdscr);
}

QStringList lines;
QStringList splitString(QString msg, int startx, int width=Window::screenWidth())
{
    QStringList tmp = msg.split("\n");
    QStringList out;
    foreach (QString str, tmp) {
        while (str.length()+startx>width)
        {            
            out.append(str.left(width-startx));
            str = str.mid(width-startx);
            startx = 0;
        }
        out.append(str);
    }
    return out;
}

void Window::write(QString msg)
{
    int startx = 0;
    if (!lines.empty())
    {
        startx= lines.last().length();
        QString tmp = lines.last();
        lines.removeLast();
        QStringList list = splitString(msg, startx);
        tmp += list.at(0);
        list.removeFirst();
        lines.append(tmp);
        lines.append(list);
    }
    else lines.append(splitString(msg, startx));
    if (lines.count()>screenHeight())
    {
        int c = lines.count();
        for (int i=screenHeight(); i<c; i++) lines.removeFirst();
    }
    clear();
    for (int i=0; i<lines.count(); i++)
    {
        mvprintw(i, 0, lines.at(i).toStdString().c_str());
    }
    refresh();
}

void Window::writeLn(QString msg)
{
    write(msg+"\n");
}

void Window::refresh()
{        
    int x=0, y=0;
    if (!lines.empty())
    {
        x=lines.last().length();
        y=lines.count()-1;
    }
    move(y, x);
    wrefresh(stdscr);
    update_panels();
    doupdate();
}
