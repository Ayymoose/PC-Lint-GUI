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
    LintManager.cpp \
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
    Lint.h \
    LintManager.h \
    Log.h \
    MainWindow.h \
    ModifiedFileThread.h \
    Preferences.h \
    ProgressWindow.h \
    ProjectSolution.h \
    atomicops.h \
    readerwriterqueue.h

FORMS += \
    MainWindow.ui \
    Preferences.ui \
    ProgressWindow.ui

RESOURCES += \
    'PC-Lint GUI'.qrc \
