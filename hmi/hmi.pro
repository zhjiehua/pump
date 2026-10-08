greaterThan(QT_MAJOR_VERSION, 4) {
    QT += core gui widgets serialport network
    CONFIG += c++14
} else {
    QT += core gui network
    CONFIG += c++11
    QMAKE_CXXFLAGS -= -std=gnu++98
    QMAKE_CXXFLAGS -= -std=c++98
    QMAKE_CXXFLAGS += -std=gnu++11
    QMAKE_CXXFLAGS += -include$$PWD/utils/qtcompat.h
}

TEMPLATE = app
TARGET = pump

DESTDIR = $$PWD/bin

# Intermediate files under the build directory (OUT_PWD), not next to sources.
OBJECTS_DIR = $$OUT_PWD/.obj
MOC_DIR     = $$OUT_PWD/.moc
RCC_DIR     = $$OUT_PWD/.rcc

# CONFIG += touch

# Feature flags: see utils/hmiconfig.h
embedded {
    DEFINES += EMBEDDED_LINUX
    QMAKE_CXXFLAGS += -fno-omit-frame-pointer -funwind-tables
    QMAKE_CFLAGS += -fno-omit-frame-pointer -funwind-tables
}

touch {
    DEFINES += HMI_INPUT_TOUCH
}

INCLUDEPATH += $$PWD/src $$PWD/components $$PWD/utils $$PWD/third_party/spdlog/include
include($$PWD/third_party/miniaes/miniaes.pri)
include($$PWD/third_party/minilzo/minilzo.pri)
include($$PWD/third_party/md5/md5.pri)
lessThan(QT_MAJOR_VERSION, 5) {
    INCLUDEPATH += $$PWD/third_party/qextserialport $$PWD/third_party/cjson
}
unix {
    LIBS += -lpthread -ldl
    QMAKE_LFLAGS += -rdynamic
}

asan {
    message("AddressSanitizer enabled")
    CONFIG += sanitizer sanitize_address
    QMAKE_CXXFLAGS += -fno-omit-frame-pointer
    QMAKE_CFLAGS += -fno-omit-frame-pointer
    DESTDIR = $$OUT_PWD
}

include($$PWD/utils/utils.pri)
include($$PWD/components/log/log.pri)
include($$PWD/components/dualbackup/dualbackup.pri)
lessThan(QT_MAJOR_VERSION, 5) {
    include($$PWD/third_party/third_party.pri)
}
include($$PWD/src/src.pri)

TRANSLATIONS += translations/hmi_zh.ts

RESOURCES += hmi.qrc
