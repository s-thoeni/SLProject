#ifndef SLSCENEBUILDERTRIANGLE_H
#define SLSCENEBUILDERTRIANGLE_H


#include <SLWindow.h>

class SLSceneBuilderTriangle : public SLSceneBuilder
{
public:
    SLSceneBuilderTriangle(){}

    virtual void build(SLScene* s, SLSceneView* sv, SLSceneID sceneID);
};

#endif // SLSCENEBUILDERTRIANGLE_H
