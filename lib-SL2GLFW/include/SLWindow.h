#ifndef SLWINDOW_H
#define SLWINDOW_H

#include <SLApplication.h>
#include <SLScene.h>
#include <SLSceneView.h>

typedef void (*onSceneChange) (SLSceneID);

struct SceneDto{
    const char* name;
    const char* info;
    SLNode* root;
};

class SLResourceLoader
{
public:
    SLResourceLoader(){}

    virtual SLbyte* getResource() = 0;
};

class SLGuiBuilder
{
public:
    SLGuiBuilder(){}

    void* buildFunction;

    virtual void registerSceneChangeListener(onSceneChange listener) = 0;

    virtual void onSceneCreated(SLuint dpi){}
    virtual void onTerminate(){}
};

class SLSceneBuilder
{
public:
    SLSceneBuilder(){}

    SLResourceLoader* resourceLoader;

    virtual void build(SLScene* s, SLSceneView* sv, SLSceneID sceneID) = 0;
    virtual void preLoadScene(SLSceneID) = 0;
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
    virtual ~SLWindow();

    SLGuiBuilder* guiBuilder;
    SLSceneBuilder* sceneBuilder;

    virtual int show();
    virtual int abstractShow() = 0;
    virtual void onSceneCreated();
    virtual void onTerminate();

protected:
    SLSceneID startScene;
    SLuint dpi;
    int width;
    int height;
};

#endif // SLWINDOW_H
