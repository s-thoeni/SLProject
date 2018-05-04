#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#include "SLWindow.h"

class GLFWWindow : public SLWindow
{
public:
    GLFWWindow(SLSceneID startScene,               
               int width = 640,
               int height = 480):
        SLWindow(startScene, 0, width, height) // dpi will be set at startup
    {}

    virtual int abstractShow();
};

#endif // GLFWWINDOW_H
