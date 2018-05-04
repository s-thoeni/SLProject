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

int SLWindow::show()
{
    // register scene changess
    slwindow_appSceneBuilder = this->sceneBuilder;    
    this->guiBuilder->registerSceneListener(&defaultSceneChangeListener);

    return this->abstractShow();
}

void SLWindow::onSceneCreated()
{
    // build first scene
    SLScene* s = SLApplication::scene;
    SLSceneView* sv = s->sv(svIndex);
    // fsb1: what is this code for?
    if (!s->root3D())
    {
        defaultSceneChangeListener(this->startScene);
    }
    else sv->onInitialize();

    // inform the GuiBuilder
    this->guiBuilder->onSceneCreated(this->dpi);
}

void SLWindow::onTerminate()
{
    this->guiBuilder->onTerminate();
}

void SLGuiBuilder::scaleImGUI(SLuint dotsPerInch){

    // Scale for proportioanl and fixed size fonts
    SLfloat dpiScaleProp = dotsPerInch / 120.0f;
    SLfloat dpiScaleFixed = dotsPerInch / 142.0f;

    // Default settings for the first time
    SLGLImGui::fontPropDots  = SL_max(16.0f * dpiScaleProp, 16.0f);
    SLGLImGui::fontFixedDots = SL_max(13.0f * dpiScaleFixed, 13.0f);

    // Adjust UI paddings on DPI
    ImGuiStyle& style = ImGui::GetStyle();
    style.FramePadding.x = SL_max(8.0f * dpiScaleFixed, 8.0f);
    style.WindowPadding.x = style.FramePadding.x;
    style.FramePadding.y = SL_max(3.0f * dpiScaleFixed, 3.0f);
    style.ItemSpacing.x = SL_max(8.0f * dpiScaleFixed, 8.0f);
    style.ItemSpacing.y = SL_max(3.0f * dpiScaleFixed, 3.0f);
    style.ItemInnerSpacing.x = style.ItemSpacing.y;

}
