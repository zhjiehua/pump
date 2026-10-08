# Qt 4: bundled serial port and JSON (see hmi.pro INCLUDEPATH)

HEADERS += \
    $$PWD/qextserialport/qextserialbase.h \
    $$PWD/qextserialport/qextserialport.h \
    $$PWD/qextserialport/posix_qextserialport.h \
    $$PWD/qextserialport/win_qextserialport.h

SOURCES += \
    $$PWD/qextserialport/qextserialbase.cpp \
    $$PWD/qextserialport/qextserialport.cpp \
    $$PWD/qextserialport/posix_qextserialport.cpp \
    $$PWD/qextserialport/win_qextserialport.cpp \
    $$PWD/cjson/cJSON.c
