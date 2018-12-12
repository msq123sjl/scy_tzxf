#-------------------------------------------------
#
# Project created by QtCreator 2014-06-03T15:45:01
#
#-------------------------------------------------

QT       += core gui sql network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = scy
TEMPLATE = app

INCLUDEPATH += ./api ./usercontrol/

SOURCES += main.cpp\
    frmconfig.cpp \
    api/myapp.cpp \
    qextserial/qextserialport.cpp \
    frmlogin.cpp \
    qextserial/qextserialport_unix.cpp \
    usercontrol/myswitchbutton.cpp \
    frminput.cpp \
    frmevent.cpp \
    frmdata.cpp \
    api/mysqliteapi.cpp \
    api/myapi.cpp \
    api/mythread.cpp \
    api/message.cpp \
    frmsampler.cpp \
    api/gpio.cpp \
    frmvalve.cpp \
    frmdiagnose.cpp \
    api/spi_drivers.cpp \
    frmmain.cpp \
    frmcod.cpp \
    api/valve_control.cpp

HEADERS  += \
    frmconfig.h \
    api/myapp.h \
    api/myhelper.h \
    qextserial/qextserialport.h \
    qextserial/qextserialport_global.h \
    qextserial/qextserialport_p.h \
    frmlogin.h \
    usercontrol/myswitchbutton.h \
    frminput.h \
    frmevent.h \
    frmdata.h \
    api/mysqliteapi.h \
    api/myapi.h \
    api/mythread.h \
    api/message.h \
    api/em335x_drivers.h \
    frmsampler.h \
    api/gpio.h \
    frmvalve.h \
    frmdiagnose.h \
    api/spi_drivers.h \
    frmmain.h \
    frmcod.h \
    api/valve_control.h

FORMS    += \
    frmconfig.ui \
    frmdata.ui \
    frmlogin.ui \
    frmnum.ui \
    frminput.ui \
    frmevent.ui \
    frmsampler.ui \
    frmvalve.ui \
    frmdiagnose.ui \
    frmmain.ui \
    frmhardwaretest.ui \
    frmcod.ui

DESTDIR=bin

RESOURCES += \
    main.qrc

RC_FILE += main.rc

OTHER_FILES += \
    abstract.txt
