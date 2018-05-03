#ifndef MINIMALSCENEBUILDER_H
#define MINIMALSCENEBUILDER_H

#include <SLWindow.h>

class SLSceneBuilderMinimal : public SLSceneBuilder
{
public:
    SLSceneBuilderMinimal();

    virtual void build(SLScene* s, SLSceneView* sv, SLSceneID sceneID);
};

#endif // MINIMALSCENEBUILDER_H
