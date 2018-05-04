#ifndef MINIMALGUIBUILDER_H
#define MINIMALGUIBUILDER_H

#include <SLWindow.h>

#include <SLApplication.h>
#include <SLScene.h>
#include <SLSceneView.h>

class SLGuiBuilderMinimal : public SLGuiBuilder
{
public:
    SLGuiBuilderMinimal();

    virtual void registerSceneListener(onSceneChange listener);
    virtual void onSceneCreated(SLuint dpi);
};

#endif // MINIMALGUIBUILDER_H
