#include <QCoreApplication>
#include <QDebug>

#include "window.h"
#include "ncurses.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Window::Initialize();
    Window::write("dupa blada ");
    Window::writeLn("zjadla schaba");
    Window::writeLn("test");
    Window::writeLn("test 2");
    Window::writeLn("test 3");
    Window::refresh();
    for (int i=0;i<50;i++)
    {
        Window::writeLn("0123456789abcdefgh");
    }
    Window::writeLn("test 4");

    return a.exec();
}
