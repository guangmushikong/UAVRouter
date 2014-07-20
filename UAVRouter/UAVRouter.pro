#-------------------------------------------------
#
# Project created by QtCreator 2013-06-23T16:49:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UAVRouter
TEMPLATE = app

RC_ICONS = guangmu_128.ico

INCLUDEPATH = .

INCLUDEPATH += F:/Win_32bit/gdal/Release/include
INCLUDEPATH += ./niGeom

LIBS += F:/Win_32bit/gdal/Release/lib/gdal_i.lib

SOURCES += main.cpp\
        mainwindow.cpp \
    flightroutedesign.cpp \
    polygonareaflightroutedesign.cpp \
    linearflightroutedesign.cpp \
    designtaskfactory.cpp \
    flightparameter.cpp \
    cogrgeometryfilereader.cpp \
    gomologging.cpp \
    uavrouteoutputer.cpp \
    geomertyconvertor.cpp \
    UAVRoute.cpp \
    coordinateoutput.cpp \
    multiregiondesigner.cpp \
    flightparameterinput.cpp \
    uicontroller.cpp \
    GomoGemetry2D.cpp \
    ./niGeom/source/niPolygon2d.cpp \
    ./niGeom/source/niPolygon2dFn.cpp \
    ./niGeom/source/niGeomMath2d.cpp

HEADERS  += mainwindow.h \
    flightroutedesign.h \
    polygonareaflightroutedesign.h \
    linearflightroutedesign.h \
    designtaskfactory.h \
    flightparameter.h \
    GomoPhotoBasic.h \
    cogrgeometryfilereader.h \
    gomologging.h \
    GomoGeometry2D.h \
    UAVRoute.h \
    uavrouteoutputer.h \
    geomertyconvertor.h \
    coordinateoutput.h \
    multiregiondesigner.h \
    flightparameterinput.h \
    uicontroller.h

FORMS    += mainwindow.ui
