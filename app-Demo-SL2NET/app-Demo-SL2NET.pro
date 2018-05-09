##############################################################################
#  File:      app-Demo-SL2GLFW.pro
#  Purpose:   QMake project definition file for a demo of SL with GLFW
#  Author:    Benjamin Fankhauser
#  Date:      Mai 2018
#  Copyright: Marcus Hudritsch, Switzerland
#             THIS SOFTWARE IS PROVIDED FOR EDUCATIONAL PURPOSE ONLY AND
#             WITHOUT ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED.
##############################################################################

TEMPLATE = app
TARGET = app-Demo-SL2NET

CONFIG += console
CONFIG += app_bundle
CONFIG -= qt
CONFIG += warn_off

DESTDIR     = ../_bin-$$CONFIGURATION-$$PLATFORM
OBJECTS_DIR = ../intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM

QMAKE_CXXFLAGS += -std=c++11

HEADERS += \

SOURCES += \
    NetDemo.cpp

