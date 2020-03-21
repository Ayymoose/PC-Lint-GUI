TEMPLATE = app
TARGET = notepad

qtHaveModule(printsupport): QT += printsupport
requires(qtConfig(fontdialog))

SOURCES += \
    LintOptions.cpp \
    main.cpp\
    notepad.cpp

HEADERS += notepad.h \
    LintOptions.h

FORMS += notepad.ui \
    LintOptions.ui

RESOURCES += \
    notepad.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/tutorials/notepad
INSTALLS += target

