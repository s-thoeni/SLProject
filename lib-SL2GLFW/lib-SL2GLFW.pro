##############################################################################
#  File:      lib-SL2GLFW.pro
#  Purpose:   Use this library in order to start with GLFW
#  Author:    Benjamin Fankhauser
#  Date:      Mai 2018
#  Copyright: Marcus Hudritsch, Switzerland
#             THIS SOFTWARE IS PROVIDED FOR EDUCATIONAL PURPOSE ONLY AND
#             WITHOUT ANY WARRANTIES WHETHER EXPRESSED OR IMPLIED.
##############################################################################

TEMPLATE = lib
TARGET = lib-SL2GLFW

CONFIG += staticlib
CONFIG -= qt
CONFIG += glfw
CONFIG += warn_off

#define platform variable for folder name
win32 {contains(QMAKE_TARGET.arch, x86_64) {PLATFORM = x64} else {PLATFORM = Win32}}
macx {PLATFORM = macx}
unix:!macx:!android {PLATFORM = linux}
android {PLATFORM = android}
#define configuration variable for folder name
CONFIG(debug, debug|release) {CONFIGURATION = Debug} else {CONFIGURATION = Release}

DESTDIR     = ../_lib/$$CONFIGURATION/$$PLATFORM
OBJECTS_DIR = ../intermediate/$$TARGET/$$CONFIGURATION/$$PLATFORM

win32 {
  POST_TARGETDEPS += ../_lib/$$CONFIGURATION/$$PLATFORM/lib-SLExternal.lib
  POST_TARGETDEPS += ../_lib/$$CONFIGURATION/$$PLATFORM/lib-assimp.lib
} else {
  POST_TARGETDEPS += ../_lib/$$CONFIGURATION/$$PLATFORM/liblib-SLExternal.a
  POST_TARGETDEPS += ../_lib/$$CONFIGURATION/$$PLATFORM/liblib-assimp.a
}

include(../SLProjectCommon.pro)
include(../SLProjectCommonLibraries.pro)
INCLUDEPATH += source

HEADERS += \
    include/SLWindow.h \
    include/GLFWWindow.h \
    include/SLWindowAndroid.h


SOURCES += \
    source/GLFWWindow.cpp \
    source/SLWindow.cpp \
    source/SLWindowAndroid.cpp

