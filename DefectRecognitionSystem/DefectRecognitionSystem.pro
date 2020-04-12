#-------------------------------------------------
#
# Project created by QtCreator 2020-04-08T01:30:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DefectRecognitionSystem
TEMPLATE = app

UI_DIR  = obj/Gui
MOC_DIR = obj/Moc
OBJECTS_DIR = obj/Obj


# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DEFINES -= UNICODE

DEFINES -= _UNICODE

CONFIG(release, debug | release){
    DESTDIR = $$PWD/../Bins/Release
}else {
    DESTDIR = $$PWD/../Bins/Debug
}


CONFIG += c++17

INCLUDEPATH += ..\Dependencies\dcmtk-3.6.5\dcmdata\include

INCLUDEPATH += ..\Dependencies\cximage\include

INCLUDEPATH += ..\LibImgIO

INCLUDEPATH += ..\LibImgProc




SOURCES += \
        main.cpp \
    mainwindow.cpp \
    recognizewdg.cpp \
    recheckwdg.cpp \
    readdcmfile.cpp \
    Base/FunctionTransfer.cpp \
    appconfig.cpp \
    geometryItem/dragitem.cpp \
    geometryItem/ellipseitem.cpp \
    geometryItem/geometryitembase.cpp \
    geometryItem/linedragitemdata.cpp \
    geometryItem/lineitem.cpp \
    geometryItem/polygonitem.cpp \
    geometryItem/rectdragitemdata.cpp \
    geometryItem/rectitem.cpp \
    geometryItem/textitem.cpp \
    logindlg.cpp \
    prewdg.cpp

HEADERS += \
    mainwindow.h \
    recognizewdg.h \
    recheckwdg.h \
    readdcmfile.h \
    Base/FunctionTransfer.h \
    appconfig.h \
    geometryItem/dragitem.h \
    geometryItem/ellipseitem.h \
    geometryItem/geometryitembase.h \
    geometryItem/linedragitemdata.h \
    geometryItem/lineitem.h \
    geometryItem/polygonitem.h \
    geometryItem/rectdragitemdata.h \
    geometryItem/rectitem.h \
    geometryItem/textitem.h \
    logindlg.h \
    prewdg.h

FORMS += \
    mainwindow.ui \
    recognizewdg.ui \
    recheckwdg.ui \
    logindlg.ui \
    prewdg.ui

CONFIG(release, debug|release) {
    LIBS += -L"..\Dependencies\dcmtk-3.6.5\lib\Release" -ldcmtk
   # LIBS += -L"..\Dependencies\cximage\lib\Release" -lcximagecrt
    LIBS += -L"..\Bins\Release" -lLibImgIO
    LIBS += -L"..\Bins\Release" -lLibImgProc


}
else {
    LIBS += -L"..\Dependencies\dcmtk-3.6.5\lib\Debug" -ldcmtk
   # LIBS += -L"..\Dependencies\cximage\lib\Debug" -lcximagecrt
    LIBS += -L"..\Bins\Debug" -lLibImgIO.
    LIBS += -L"..\Bins\Debug" -lLibImgProc

}

RESOURCES += \
    imgs.qrc \
    qss.qrc
