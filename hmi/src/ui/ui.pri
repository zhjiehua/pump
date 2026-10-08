HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/focuspage.h \
    $$PWD/topbar.h \
    $$PWD/bottombar.h

SOURCES += \
    $$PWD/mainwindow.cpp \
    $$PWD/focuspage.cpp \
    $$PWD/topbar.cpp \
    $$PWD/bottombar.cpp

include($$PWD/widgets/widgets.pri)
include($$PWD/pages/pages.pri)
