HEADERS += \
    $$PWD/crc16.h \
    $$PWD/version.h \
    $$PWD/qtcompat.h \
    $$PWD/configpaths.h \
    $$PWD/qjsonshim.h \
    $$PWD/qtwidgetsutil.h \
    $$PWD/spatialfocus.h \
    $$PWD/eventlog.h \
    $$PWD/hmiconfig.h \
    $$PWD/hmikeys.h

SOURCES += \
    $$PWD/crc16.cpp \
    $$PWD/spatialfocus.cpp \
    $$PWD/eventlog.cpp

lessThan(QT_MAJOR_VERSION, 5) {
    SOURCES += $$PWD/qjsonshim.cpp $$PWD/configpaths.cpp
} else {
    SOURCES += $$PWD/configpaths.cpp
}
