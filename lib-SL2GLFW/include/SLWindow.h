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

    virtual void onSceneCreated(int dpi){}
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
    SLWindow(SLSceneID startScene,
             int width,
             int height):
        startScene(startScene),
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

    virtual int show(int argc, char *argv[]);
    virtual int abstractShow(int argc, char *argv[]) = 0;
    virtual void onSceneCreated(SLuint svIndex, SLuint dpi);

protected:
    SLSceneID startScene;
    int width;
    int height;
};

#endif // SLWINDOW_H
