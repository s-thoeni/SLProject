#ifndef SLSCENEBUILDERDEMO_H
#define SLSCENEBUILDERDEMO_H

#include "SLWindow.h"

class SLSceneBuilderDemo : public SLSceneBuilder
{
public:
    SLSceneBuilderDemo();

    virtual void build(SLScene* s, SLSceneView* sv, SLSceneID sceneID);
};

#endif // SLSCENEBUILDERDEMO_H
