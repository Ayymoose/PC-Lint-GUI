TEMPLATE = app
TARGET = linty

qtHaveModule(printsupport): QT += printsupport
requires(qtConfig(fontdialog))

SOURCES += \
    LintOptions.cpp \
    MainWindow.cpp \
    Settings.cpp \
    main.cpp

HEADERS += \
    LintOptions.h \
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

