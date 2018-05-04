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

SLWindow::~SLWindow(){
    if (this->guiBuilder)
        delete this->guiBuilder;
    if (this->sceneBuilder)
        delete this->sceneBuilder;
}

int SLWindow::show()
{
    // register scene builder to the gui events
    slwindow_appSceneBuilder = this->sceneBuilder;    
    this->guiBuilder->registerSceneChangeListener(&defaultSceneChangeListener);

    return this->abstractShow();
}

void SLWindow::onSceneCreated()
{
    // build first scene
    SLScene* s = SLApplication::scene;
    SLSceneView* sv = s->sv(svIndex);

    // initialize first scene:
    defaultSceneChangeListener(this->startScene);

    // inform the GuiBuilder
    this->guiBuilder->onSceneCreated(this->dpi);
}

void SLWindow::onTerminate()
{
    this->guiBuilder->onTerminate();
}
