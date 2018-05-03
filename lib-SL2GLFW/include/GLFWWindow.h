#ifndef GLFWWINDOW_H
#define GLFWWINDOW_H

#include "SLWindow.h"

class GLFWWindow : public SLWindow
{
public:
    GLFWWindow(SLSceneID startScene,
               int width = 640,
               int height = 480):
        SLWindow(startScene, width, height)
    {}

    virtual int abstractShow(int argc, char *argv[]);
};

#endif // GLFWWINDOW_H
