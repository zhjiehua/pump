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

embedded {
    DEFINES += EMBEDDED_LINUX
}

INCLUDEPATH += $$PWD/src $$PWD/components $$PWD/utils $$PWD/third_party/spdlog/include
lessThan(QT_MAJOR_VERSION, 5) {
    INCLUDEPATH += $$PWD/third_party/qextserialport $$PWD/third_party/cjson
}
unix: LIBS += -lpthread

asan {
    message("AddressSanitizer enabled")
    CONFIG += sanitizer sanitize_address
    QMAKE_CXXFLAGS += -fno-omit-frame-pointer
    QMAKE_CFLAGS += -fno-omit-frame-pointer
    DESTDIR = $$OUT_PWD
}

lessThan(QT_MAJOR_VERSION, 5) {
    HEADERS += \
        third_party/qextserialport/qextserialbase.h \
        third_party/qextserialport/qextserialport.h \
        third_party/qextserialport/posix_qextserialport.h \
        third_party/qextserialport/win_qextserialport.h \
        utils/qjsonshim.h

    SOURCES += \
        third_party/qextserialport/qextserialbase.cpp \
        third_party/qextserialport/qextserialport.cpp \
        third_party/qextserialport/posix_qextserialport.cpp \
        third_party/qextserialport/win_qextserialport.cpp \
        third_party/cjson/cJSON.c \
        utils/qjsonshim.cpp \
        utils/configpaths.cpp
} else {
    SOURCES += utils/configpaths.cpp
}

TRANSLATIONS += translations/hmi_zh.ts

HEADERS += \
    utils/crc16.h \
    utils/version.h \
    utils/qtcompat.h \
    utils/configpaths.h \
    utils/qjsonshim.h \
    utils/qtwidgetsutil.h \
    utils/spatialfocus.h \
    utils/eventlog.h \
    components/log/log.h \
    components/dualbackup/jsondualbackup.h \
    src/core/appsettings.h \
    src/core/alarmservice.h \
    src/core/authservice.h \
    src/core/gradientengine.h \
    src/core/runstatemachine.h \
    src/core/usagetracker.h \
    src/core/commworker.h \
    src/core/i18nmanager.h \
    src/core/picturemanager.h \
    src/core/buglecompensation.h \
    src/core/calibinterp.h \
    src/core/machinecontroller.h \
    src/platform/platform.h \
    src/platform/iomodule.h \
    src/platform/hmiserialport.h \
    src/protocol/pc/pcserver.h \
    src/protocol/pc/cxth/cxthpcids.h \
    src/protocol/pc/cxth/cxthpccodec.h \
    src/protocol/pc/cxth/cxthpcserver.h \
    src/protocol/pc/clarity/clarityids.h \
    src/protocol/pc/clarity/claritycodec.h \
    src/protocol/pc/clarity/claritypcserver.h \
    src/protocol/pc/qinfine/qinfinepcserver.h \
    src/protocol/mcu/cxth/cxthmcucodec.h \
    src/protocol/mcu/cxth/cxthmcuclient.h \
    src/protocol/mcu/qinfine/qinfinecodec.h \
    src/protocol/mcu/qinfine/qinfineclient.h \
    src/ui/mainwindow.h \
    src/ui/focuspage.h \
    utils/hmikeys.h \
    src/ui/topbar.h \
    src/ui/bottombar.h \
    src/ui/widgets/keyboarddialog.h \
    src/ui/widgets/editctrl.h \
    src/ui/widgets/comboctrl.h \
    src/ui/widgets/btnctrl.h \
    src/ui/widgets/imgbutton.h \
    src/ui/widgets/hmitablewidget.h \
    src/ui/widgets/tablecelleditor.h \
    src/ui/widgets/tableitemdelegate.h \
    src/ui/pages/logopage.h \
    src/ui/pages/runpage.h \
    src/ui/pages/runparampage.h \
    src/ui/pages/setuppage.h \
    src/ui/pages/fixpage.h \
    src/ui/pages/flowfixpage.h \
    src/ui/pages/pressfixpage.h \
    src/ui/pages/adminpage.h \
    src/ui/pages/netpage.h \
    src/ui/pages/internalconfigpage.h \
    src/ui/pages/languagepage.h \
    src/ui/pages/timepage.h \
    src/ui/pages/msgpage.h \
    src/ui/pages/permitpage.h \
    src/ui/pages/glpinfopage.h \
    src/ui/pages/pwdpage.h \
    src/ui/pages/gradienttablepage.h \
    src/ui/pages/debugmcuprotopage.h

SOURCES += \
    src/main.cpp \
    utils/crc16.cpp \
    utils/spatialfocus.cpp \
    utils/eventlog.cpp \
    components/log/log.cpp \
    components/dualbackup/jsondualbackup.cpp \
    src/core/appsettings.cpp \
    src/core/alarmservice.cpp \
    src/core/authservice.cpp \
    src/core/gradientengine.cpp \
    src/core/runstatemachine.cpp \
    src/core/usagetracker.cpp \
    src/core/commworker.cpp \
    src/core/i18nmanager.cpp \
    src/core/picturemanager.cpp \
    src/core/buglecompensation.cpp \
    src/core/calibinterp.cpp \
    src/core/machinecontroller.cpp \
    src/platform/platform.cpp \
    src/platform/iomodule.cpp \
    src/platform/hmiserialport.cpp \
    src/protocol/pc/pcserver.cpp \
    src/protocol/pc/cxth/cxthpccodec.cpp \
    src/protocol/pc/cxth/cxthpcserver.cpp \
    src/protocol/pc/clarity/claritycodec.cpp \
    src/protocol/pc/clarity/claritypcserver.cpp \
    src/protocol/pc/qinfine/qinfinepcserver.cpp \
    src/protocol/mcu/cxth/cxthmcucodec.cpp \
    src/protocol/mcu/cxth/cxthmcuclient.cpp \
    src/protocol/mcu/qinfine/qinfinecodec.cpp \
    src/protocol/mcu/qinfine/qinfineclient.cpp \
    src/ui/mainwindow.cpp \
    src/ui/focuspage.cpp \
    src/ui/topbar.cpp \
    src/ui/bottombar.cpp \
    src/ui/widgets/keyboarddialog.cpp \
    src/ui/widgets/editctrl.cpp \
    src/ui/widgets/comboctrl.cpp \
    src/ui/widgets/btnctrl.cpp \
    src/ui/widgets/imgbutton.cpp \
    src/ui/widgets/hmitablewidget.cpp \
    src/ui/widgets/tablecelleditor.cpp \
    src/ui/widgets/tableitemdelegate.cpp \
    src/ui/pages/logopage.cpp \
    src/ui/pages/runpage.cpp \
    src/ui/pages/runparampage.cpp \
    src/ui/pages/setuppage.cpp \
    src/ui/pages/fixpage.cpp \
    src/ui/pages/flowfixpage.cpp \
    src/ui/pages/pressfixpage.cpp \
    src/ui/pages/adminpage.cpp \
    src/ui/pages/netpage.cpp \
    src/ui/pages/internalconfigpage.cpp \
    src/ui/pages/languagepage.cpp \
    src/ui/pages/timepage.cpp \
    src/ui/pages/msgpage.cpp \
    src/ui/pages/permitpage.cpp \
    src/ui/pages/glpinfopage.cpp \
    src/ui/pages/pwdpage.cpp \
    src/ui/pages/gradienttablepage.cpp \
    src/ui/pages/debugmcuprotopage.cpp

RESOURCES += hmi.qrc
