#include "widget.h"
#include "window.h"
#include "ncurses.h"
#include "panel.h"
#include <QString>
#include <QStringList>
#include "textwidget.h"
#include "buttonboxwidget.h"

QHash<panel*, Window*> Window::_windows;

void Window::Initialize()
{
    initscr();
    keypad(stdscr, true);
    noecho();
    raw();    
    start_color();
    init_pair((short)ColorWhiteOnBlack, COLOR_BLACK, COLOR_WHITE);
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
        for (int i=index+1; i<_widgets.count(); i++)
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
    widget->setParent(this);
    if (!_activeWidget)
    {
        if (widget->isFocusable()) _activeWidget = widget;
    }
}

_win_st *Window::window() const
{
    return _window;
}

int Window::showMessageBox(QString title, QString message, QStringList buttons)
{
    int width = screenWidth()/2;
    QStringList tmp = splitString(message, width-2);
    int height = tmp.count();
    Window* wnd= new Window(height+3, width);
    wnd->title = title;
    TextWidget* text = new TextWidget(height, TextWidget::AlginCenter);
    wnd->addWidget(text);
    ButtonBoxWidget* butWidget = new ButtonBoxWidget(buttons, 7);
    wnd->addWidget(butWidget);
    butWidget->focus(true);
    text->setText(message);
    wnd->draw();    
    while (true)
    {
        int c = Window::getCh();
        if (wnd->pressKey(c)) wnd->draw();
    }
    return 0;
}

int Window::getCh()
{
    return getch();
}

Window::Window(int height, int width, int row, int column)
{
    _color = Window::ColorNone;
    _window = newwin(height, width, row, column);
    box(_window, 0, 0);
    _panel = new_panel(_window);
    _windows.insert(_panel, this);
    _activeWidget = 0;
}

Window::Window(int height, int width) : Window(height, width, (screenHeight()-height)/2, (screenWidth()-width)/2)
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
    wclear(_window);
    box(_window, 0, 0);
    if (title.length()>width()-2)
    {
        mvwprintw(_window, 0, 1, title.left(width()-2).toStdString().c_str());
    }
    else
    {
        int x = (width()-title.length())/2;
        mvwprintw(_window, 0, x, title.toStdString().c_str());
    }
    int row = 1;
    foreach (Widget* w, _widgets) {
        row = w->draw(row, 1);
        if (row>height()) break;
    }

    refresh();
}

bool Window::pressKey(int key)
{
    switch (key) {
    case KEY_UP:
        if (_activeWidget) return _activeWidget->keyUpPressed();
        break;
    case KEY_DOWN:
        if (_activeWidget) return _activeWidget->keyDownPressed();
        break;
    case KEY_BTAB:
        return focusNextWidget(true);
        break;
    default:
        if (_activeWidget) return _activeWidget->keyPressed(key);
        break;
    }
    return false;
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
QStringList splitStringHard(QString msg, int startx, int width=Window::screenWidth())
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
        QStringList list = splitStringHard(msg, startx);
        tmp += list.at(0);
        list.removeFirst();
        lines.append(tmp);
        lines.append(list);
    }
    else
    {
        lines.append(splitStringHard(msg, startx));
    }
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
    update_panels();
    doupdate();
    move(y, x);
    wrefresh(stdscr);
}

void Window::colorOn(Window::Color c)
{
    if (_color!=ColorNone)
    {
        wattroff(_window, COLOR_PAIR((int)_color));
    }
    _color = c;
    wattron(_window, COLOR_PAIR((int)_color));
}

void Window::colorOff()
{
    if (_color!=ColorNone)
    {
        wattroff(_window, COLOR_PAIR((int)_color));
    }
}

QStringList &Window::splitString(QString msg, int width, int maxHeight)
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
            if (maxHeight!=-1 && out->count()>maxHeight) return *out;
        }
        out->append(str);
        if (maxHeight!=-1 && out->count()>maxHeight) return *out;
    }
    return *out;
}
