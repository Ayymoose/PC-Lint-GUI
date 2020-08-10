TEMPLATE = app
TARGET = linty

qtHaveModule(printsupport): QT += printsupport xml
requires(qtConfig(fontdialog))

SOURCES += \
    CodeEditor.cpp \
    Highlighter.cpp \
    LintOptions.cpp \
    LintThreadManager.cpp \
    Linter.cpp \
    Log.cpp \
    MainWindow.cpp \
    ModifiedFileThread.cpp \
    ProgressWindow.cpp \
    ProjectSolution.cpp \
    Settings.cpp \
    main.cpp

HEADERS += \
    CodeEditor.h \
    Highlighter.h \
    Jenkins.h \
    LintOptions.h \
    LintThreadManager.h \
    Linter.h \
    Log.h \
    MainWindow.h \
    ModifiedFileThread.h \
    ProgressWindow.h \
    ProjectSolution.h \
    Settings.h

FORMS += \
    LintOptions.ui \
    MainWindow.ui \
    ProgressWindow.ui

RESOURCES += \
    linty.qrc \

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/tutorials/notepad
INSTALLS += target

DISTFILES +=

