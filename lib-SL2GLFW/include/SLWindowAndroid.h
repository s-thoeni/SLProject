#ifndef SLWINDOW_ANDROID_H
#define SLWINDOW_ANDROID_H

#include "SLWindow.h"
#include <string>

class SLWindowAndroid : public SLWindow
{
public:
    SLWindowAndroid(string devicePath,                    
                    void* onWndUpdateCallback,                    
                    SLSceneID startScene,
                    SLuint dpi,
                    int width,
                    int height):
        SLWindow(startScene, dpi, width, height),
        devicePath(devicePath),        
        onWndUpdateCallback(onWndUpdateCallback)
    {}

    virtual int abstractShow();
private:
    string devicePath;    
    void* onWndUpdateCallback;
};

#endif // SLWINDOW_ANDROID_H
