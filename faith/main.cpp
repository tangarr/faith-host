#include <QCoreApplication>
#include <QDebug>

#include "window.h"
#include "ncurses.h"

#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Window::Initialize();

    Window::refresh();

    Window::write("dupa blada ");
    Window::writeLn("zjadla schaba");
    Window::writeLn("test");
    Window::writeLn("test 2");
    Window::writeLn("test 3");
    for (int i=0;i<50;i++)
    {
        Window::write("0123456789abcdefgh");
    }
    Window::writeLn("test 4");
    //Window::showMessageBox("Uwaga", "Testuje sobie\njakas wiadomosc tekstowa", {"OK", "Yes", "No", "Cancel", "Return", "Test", "Fuck"});
    QStringList lab({"jeden", "dwa", "trzy", "laba"});
    Window::showComuterLabWindow(lab);
    return a.exec();
}
