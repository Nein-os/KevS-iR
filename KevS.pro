QT       += core gui
QT	     += gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

CONFIG += fontAwesomeFree
include(QtAwesome/QtAwesome.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    calculators/additionalinformationcalculator.cpp \
    calculators/fuelcalculator.cpp \
    calculators/fuelrecorder.cpp \
    calculators/lapcalculator.cpp \
    calculators/laprecorder.cpp \
    irsdk/irsdk_utils.cpp \
    irsdk/lap_timing.cpp \
    irsdk/yaml_parser.cpp \
    ui/additionalinformationeditwindow.cpp \
    ui/additionalinformationwindow.cpp \
    ui/debugwindow.cpp \
    ui/fueleditwindow.cpp \
    ui/fuelwindow.cpp \
    irsdk/irsdk_client.cpp \
    main.cpp \
    ui/optionwindow.cpp \
    calculators/statrecorder.cpp

HEADERS += \
    calculators/additionalinformationcalculator.h \
    calculators/fuelcalculator.h \
    calculators/fuelrecorder.h \
    calculators/lapcalculator.h \
    calculators/laprecorder.h \
    helper.h \
    irsdk/irsdk_defines.h \
    irsdk/lap_timing.h \
    irsdk/yaml_parser.h \
    ui/additionalinformationeditwindow.h \
    ui/additionalinformationwindow.h \
    ui/debugwindow.h \
    ui/fueleditwindow.h \
    ui/fuelwindow.h \
    irsdk/irsdk_client.h \
    ui/optionwindow.h \
    calculators/statrecorder.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
	LICENSE

FORMS +=
