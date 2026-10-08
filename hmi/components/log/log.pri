HEADERS += $$PWD/log.h
SOURCES += $$PWD/log.cpp

unix {
    HEADERS += $$PWD/crashhandler.h
    SOURCES += $$PWD/crashhandler.cpp
}
