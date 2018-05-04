#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#include "SLWindow.h"

class SLWindowGlfw : public SLWindow
{
public:
    SLWindowGlfw(SLSceneID startScene,
               int width = 640,
               int height = 480):
        SLWindow(startScene, 0, width, height) // dpi will be set during startup
    {}

    virtual int abstractShow();
};

#endif // GLFWWINDOW_H
