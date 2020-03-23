TEMPLATE = app
TARGET = linty

qtHaveModule(printsupport): QT += printsupport xml
requires(qtConfig(fontdialog))

SOURCES += \
    CodeEditor.cpp \
    Highlighter.cpp \
    Icon.cpp \
    LintOptions.cpp \
    Linter.cpp \
    Log.cpp \
    MainWindow.cpp \
    Settings.cpp \
    main.cpp

HEADERS += \
    CodeEditor.h \
    Highlighter.h \
    Icon.h \
    LintOptions.h \
    Linter.h \
    Log.h \
    MainWindow.h \
    Settings.h

FORMS += \
    LintOptions.ui \
    MainWindow.ui

RESOURCES += \
    linty.qrc \
    linty.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/tutorials/notepad
INSTALLS += target

DISTFILES +=

