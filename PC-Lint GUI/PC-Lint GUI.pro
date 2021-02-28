TEMPLATE = app
TARGET = 'PC-Lint GUI'

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CXXFLAGS += -Wall -Wextra -Wundef -Wpointer-arith -Wlogical-op -g

QT += xml widgets

SOURCES += \
    About.cpp \
    CodeEditor.cpp \
    Highlighter.cpp \
    LintThreadManager.cpp \
    Linter.cpp \
    Log.cpp \
    MainWindow.cpp \
    ModifiedFileThread.cpp \
    Preferences.cpp \
    ProgressWindow.cpp \
    ProjectSolution.cpp \
    Main.cpp

HEADERS += \
    About.h \
    CodeEditor.h \
    Compiler.h \
    Highlighter.h \
    Jenkins.h \
    LintThreadManager.h \
    Linter.h \
    Log.h \
    MainWindow.h \
    ModifiedFileThread.h \
    Preferences.h \
    ProgressWindow.h \
    ProjectSolution.h

FORMS += \
    MainWindow.ui \
    Preferences.ui \
    ProgressWindow.ui

RESOURCES += \
    'PC-Lint GUI'.qrc \
