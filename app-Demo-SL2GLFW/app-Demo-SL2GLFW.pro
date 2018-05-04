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
TARGET = app-Demo-SL2GLFW

CONFIG += desktop
CONFIG += console
CONFIG += app_bundle
CONFIG -= qt
CONFIG += warn_off

DEFINES += "SL_STARTSCENE=SID_Minimal"

include(../SLProjectCommon.pro)

INCLUDEPATH += ../lib-SL2GLFW/include

DESTDIR     = ../_bin-$$CONFIGURATION-$$PLATFORM
OBJECTS_DIR = ../intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM

LIBS += -L$$PWD/../_lib/$$CONFIGURATION/$$PLATFORM -llib-SL2GLFW
LIBS += -L$$PWD/../_lib/$$CONFIGURATION/$$PLATFORM -llib-SLProject
LIBS += -L$$PWD/../_lib/$$CONFIGURATION/$$PLATFORM -llib-SLExternal
LIBS += -L$$PWD/../_lib/$$CONFIGURATION/$$PLATFORM -llib-assimp

include(../SLProjectCommonLibraries.pro)
include(../SLProjectDeploy.pro)

HEADERS += \
    source/SLGuiBuilderMinimal.h \
    source/SLSceneBuilderMinimal.h \
    source/SLSceneBuilderDemo.h \
    source/SLGuiBuilderDemo.h \
    source/SLSceneBuilderTriangle.h


SOURCES += \
    source/SLGuiBuilderMinimal.cpp \
    source/SLSceneBuilderMinimal.cpp \
    source/SLSceneBuilderDemo.cpp \
    source/SLGuiBuilderDemo.cpp \
    source/SLDemo.cpp \
    source/SLSceneBuilderTriangle.cpp

