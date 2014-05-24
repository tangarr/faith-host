#-------------------------------------------------
#
# Project created by QtCreator 2014-05-24T12:51:34
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = Faith
CONFIG   += console c++11
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    window.cpp \
    widget.cpp \
    textwidget.cpp \
    buttonboxwidget.cpp \
    menuwidget.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../faith-core/release/ -lfaith-core
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../faith-core/debug/ -lfaith-core
else:unix: LIBS += -L$$OUT_PWD/../faith-core/ -lfaith-core

INCLUDEPATH += $$PWD/../faith-core
DEPENDPATH += $$PWD/../faith-core

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../faith-core/release/libfaith-core.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../faith-core/debug/libfaith-core.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../faith-core/release/faith-core.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../faith-core/debug/faith-core.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../faith-core/libfaith-core.a

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += ncurses panel

OTHER_FILES += \
    ../README

HEADERS += \
    window.h \
    widget.h \
    textwidget.h \
    buttonboxwidget.h \
    menuwidget.h
