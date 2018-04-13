#!/bin/bash

# Compiling lib-SLProject as shared library
g++ -c -O0 -I./include -I./lib-SLExternal -std=c++11 -g -o ./intermediate/liblib-SLProject.o lib-SLProject/source/SLProject.cpp -g
ar rvs ./_lib/Debug/linux/liblib-SLProject.a ./intermediate/liblib-SLProject.o

# Compiling app-Demo-GLFW
g++ -O0 -I./include -I./lib-SLExternal -I/usr/local/include -L./_lib/Debug/linux -L/usr/local/lib64 -DSL_STARTSCENE=SID_Minimal -std=c++11 -g -o app-Demo-GLFW/app-Demo-GLFW.out app-Demo-GLFW/source/AppDemoMainGLFW.cpp -lopencv_aruco -lopencv_core -lopencv_calib3d -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_objdetect -lopencv_video -lopencv_videoio -lopencv_xfeatures2d -lpthread -lGL -ldl -lX11 -lXrandr -lXi -lXinerama -lXxf86vm -lXcursor -ludev -lz -lpng -lassimp -llib-SLExternal -llib-SLProject -g
