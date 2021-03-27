TEMPLATE = app
TARGET = 'PC-Lint GUI'

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CXXFLAGS += -Wunused -Wpointer-arith -Wlogical-op

QT += xml widgets concurrent

SOURCES += \
    About.cpp \
    CodeEditor.cpp \
    Highlighter.cpp \
    Lint.cpp \
    Log.cpp \
    MainWindow.cpp \
    Preferences.cpp \
    ProgressWindow.cpp \
    Main.cpp

HEADERS += \
    About.h \
    CodeEditor.h \
    Compiler.h \
    Highlighter.h \
    Jenkins.h \
    Lint.h \
    Log.h \
    MainWindow.h \
    Preferences.h \
    ProgressWindow.h \
    atomicops.h \
    readerwriterqueue.h

FORMS += \
    MainWindow.ui \
    Preferences.ui \
    ProgressWindow.ui

RESOURCES += \
    'PC-Lint GUI'.qrc \
