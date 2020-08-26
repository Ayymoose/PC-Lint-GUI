TEMPLATE = app
TARGET = linty

qtHaveModule(printsupport): QT += printsupport xml
requires(qtConfig(fontdialog))

SOURCES += \
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
    main.cpp

HEADERS += \
    CodeEditor.h \
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
    linty.qrc \

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/tutorials/notepad
INSTALLS += target

DISTFILES +=

