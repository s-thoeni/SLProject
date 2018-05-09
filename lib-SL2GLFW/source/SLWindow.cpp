#include "SLWindow.h"

#include <thread>

SLSceneBuilder* slwindow_appSceneBuilder;
atomic_bool slwindow_somethingToUpdate (false);
SLSceneID slwindow_sceneIDtoUpdate = SID_Minimal;

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

void scenePreUpdate()
{
    // skip if nothing to update:
    if ( !slwindow_somethingToUpdate )
        return;

    defaultSceneChangeListener(slwindow_sceneIDtoUpdate);
    slwindow_somethingToUpdate = false;
}

void loader(SLSceneID sceneID)
{
    if ( slwindow_somethingToUpdate )
        return;

    slwindow_appSceneBuilder->preLoadScene(sceneID);
    slwindow_sceneIDtoUpdate = sceneID;
    slwindow_somethingToUpdate = true;
}

void threadedSceneChangeListener(SLSceneID sceneID)
{
    // load progress scene:
    defaultSceneChangeListener(SID_AnimationArmy);

    std::thread t (loader, sceneID);
    t.detach();
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
    this->guiBuilder->registerSceneChangeListener(&threadedSceneChangeListener);

    // register preupdates:


    return this->abstractShow();
}

void SLWindow::onSceneCreated()
{
    // build first scene
    SLScene* s = SLApplication::scene;
    SLSceneView* sv = s->sv(svIndex);

    // register pre update
    s->preUpdate((cbOnPreUpdate)&scenePreUpdate);

    // initialize first scene:
    defaultSceneChangeListener(this->startScene);

    // inform the GuiBuilder
    this->guiBuilder->onSceneCreated(this->dpi);
}

void SLWindow::onTerminate()
{
    this->guiBuilder->onTerminate();
}
