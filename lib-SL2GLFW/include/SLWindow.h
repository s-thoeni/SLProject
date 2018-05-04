#ifndef SLWINDOW_H
#define SLWINDOW_H

#include <SLApplication.h>
#include <SLScene.h>
#include <SLSceneView.h>

typedef void (*onSceneChange) (SLSceneID);

class SLGuiBuilder
{
public:
    SLGuiBuilder(){}

    void* buildFunction;

    virtual void onSceneCreated(SLuint dpi){}
    virtual void scaleImGUI(SLuint dpi);
    virtual void onTerminate(){}
    virtual void registerSceneListener(onSceneChange listener) = 0;

};

class SLSceneBuilder
{
public:
    SLSceneBuilder(){}

    virtual void build(SLScene* s, SLSceneView* sv, SLSceneID sceneID) = 0;
};

class SLWindow
{
public:

    SLuint svIndex;

    SLWindow(SLSceneID startScene,
             SLuint dpi,
             int width,
             int height):
        startScene(startScene),
        dpi(dpi),
        width(width),
        height(height)
    {}
    virtual ~SLWindow()
    {
        delete this->guiBuilder;
        delete this->sceneBuilder;
    }

    SLGuiBuilder* guiBuilder;
    SLSceneBuilder* sceneBuilder;

    virtual int show();
    virtual int abstractShow() = 0;
    virtual void onSceneCreated();
    virtual void onTerminate();

protected:
    SLSceneID startScene;
    int width;
    int height;
    SLuint dpi;
};

#endif // SLWINDOW_H
