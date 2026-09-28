# Desktop Qt5 shadow build for the legacy LC3000U HMI (weiduodianzi).
# Authoritative source list comes from weiduodianzi.vcxproj (not the stale .pri).
# Usage:
#   mkdir -p build_desktop bin && cd build_desktop
#   qmake ../weiduodianzi_desktop.pro && make -j$(nproc)

QT += core gui widgets network
CONFIG += c++11
TEMPLATE = app
TARGET = weiduodianzi

DESTDIR = $$PWD/bin
DEFINES += DESKTOP_HMI

INCLUDEPATH += $$PWD $$PWD/Communication

unix:!macx {
    # Prefer unversioned -lsqlite3 when libsqlite3-dev is installed;
    # fall back to the runtime .so.0 when only the shared library is present.
    exists(/usr/lib/$${QT_ARCH}-linux-gnu/libsqlite3.so)|exists(/usr/lib/x86_64-linux-gnu/libsqlite3.so) {
        LIBS += -lsqlite3
    } else {
        LIBS += /usr/lib/x86_64-linux-gnu/libsqlite3.so.0
    }
} else {
    LIBS += -lsqlite3
}

HEADERS += \
    AdminPage.h \
    BasePage.h \
    bottomwidget.h \
    btnctrl.h \
    buglecompensation.h \
    BugleCompensationWithPID.h \
    buglecompensationwithsection.h \
    comboctrl.h \
    Common.h \
    communicationcoupling.h \
    database.h \
    DebugMcuProtoPage.h \
    editctrl.h \
    editordelegate.h \
    FixPage.h \
    flowctrl.h \
    FlowFixPage.h \
    glpinfopage.h \
    GradientPage.h \
    GradientTable.h \
    imgbutton.h \
    iomodule.h \
    KeyboardDialog.h \
    language.h \
    lineuint.h \
    logopage.h \
    machinestat.h \
    msgbox.h \
    msgpage.h \
    mytablemodel.h \
    netconfigpage.h \
    permitpage.h \
    PressFixPage.h \
    pwdpage.h \
    runconst.h \
    runpage.h \
    runparam.h \
    SensitivePage.h \
    setuppage.h \
    sqlite3.h \
    tablectrl.h \
    tableeditor.h \
    timehelper.h \
    TimePage.h \
    topwidget.h \
    baseMainPage.h \
    Communication/Device.h \
    Communication/LogicThread.h \
    Communication/plc_instruction.h \
    Communication/posix_qextserialport.h \
    Communication/Protocol.h \
    Communication/Protocol_mcu.h \
    Communication/qextserialbase.h \
    Communication/qextserialport.h

SOURCES += \
    AdminPage.cpp \
    BasePage.cpp \
    bottomwidget.cpp \
    btnctrl.cpp \
    buglecompensation.cpp \
    BugleCompensationWithPID.cpp \
    buglecompensationwithsection.cpp \
    comboctrl.cpp \
    communicationcoupling.cpp \
    database.cpp \
    DebugMcuProtoPage.cpp \
    editctrl.cpp \
    editordelegate.cpp \
    FixPage.cpp \
    flowctrl.cpp \
    FlowFixPage.cpp \
    glpinfopage.cpp \
    GradientPage.cpp \
    GradientTable.cpp \
    imgbutton.cpp \
    iomodule.cpp \
    KeyboardDialog.cpp \
    language.cpp \
    lineuint.cpp \
    logopage.cpp \
    machinestat.cpp \
    main.cpp \
    baseMainPage.cpp \
    msgbox.cpp \
    msgpage.cpp \
    mytablemodel.cpp \
    netconfigpage.cpp \
    permitpage.cpp \
    PressFixPage.cpp \
    pwdpage.cpp \
    runconst.cpp \
    runpage.cpp \
    runparam.cpp \
    SensitivePage.cpp \
    setuppage.cpp \
    tablectrl.cpp \
    tableeditor.cpp \
    timehelper.cpp \
    TimePage.cpp \
    topwidget.cpp \
    Communication/LogicThread.cpp \
    Communication/posix_qextserialport.cpp \
    Communication/Protocol.c \
    Communication/Protocol_mcu.c \
    Communication/qextserialbase.cpp \
    Communication/qextserialport.cpp

FORMS += \
    AdminPage.ui \
    BaseMainPage.ui \
    bottomwidget.ui \
    DebugMcuProtoPage.ui \
    fixPage.ui \
    FlowFixPage.ui \
    glpinfopage.ui \
    GradientPage.ui \
    GradientTable.ui \
    keyboardDialog.ui \
    LanguagePage.ui \
    logpage.ui \
    MsgPage.ui \
    netconfigpage.ui \
    PermitPage.ui \
    pressFixPage.ui \
    PwdPage.ui \
    RunConstPage.ui \
    RunPage.ui \
    RunParamPage.ui \
    sensitivePage.ui \
    SetupPage.ui \
    timePage.ui \
    topwidget.ui

RESOURCES += weiduodianzi.qrc
TRANSLATIONS += weiduodianzi_zh.ts
