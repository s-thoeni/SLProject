#include "SLWindow.h"

SLSceneBuilder* slwindow_appSceneBuilder;

void defaultSceneChangeListener(SLSceneID sceneID){
    SLScene* s = SLApplication::scene;
    SLSceneView* sv = s->sceneViews()[0];

    SLApplication::sceneID = sceneID;

    // Initialize all preloaded stuff from SLScene
    s->init();

    ////////////////////////////////////////////////////////////////////////////
    slwindow_appSceneBuilder->build(s, sv, sceneID);

    ////////////////////////////////////////////////////////////////////////////
    // call onInitialize on all scene views to init the scenegraph and stats
    for (auto sv : s->sceneViews())
    {   if (sv != nullptr)
        {   sv->onInitialize();
        }
    }

    s->onAfterLoad();
}

int SLWindow::show(int argc, char *argv[])
{
    // register scene changess
    slwindow_appSceneBuilder = this->sceneBuilder;    
    this->guiBuilder->registerSceneListener(&defaultSceneChangeListener);

    return this->abstractShow(argc, argv);
}

void SLWindow::onSceneCreated(SLuint svIndex, SLuint dpi)
{
    // build first scene
    SLScene* s = SLApplication::scene;
    SLSceneView* sv = s->sceneViews()[svIndex];
    // fsb1: what is this code for?
    // if (!s->root3D())
    // {
    defaultSceneChangeListener(this->startScene);
    // }
    // else sv->onInitialize();

    // inform the GuiBuilder
    this->guiBuilder->onSceneCreated(dpi);
}
