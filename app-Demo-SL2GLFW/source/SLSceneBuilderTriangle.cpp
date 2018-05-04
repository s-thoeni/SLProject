#include "SLSceneBuilderTriangle.h"

#include <SLLightSpot.h>
#include <SLLight.h>
#include <SLRectangle.h>

class MyTriangle : public SLMesh
{
public:
    MyTriangle(int size, SLchar* name, SLMaterial* material):
        SLMesh(name)
    {
        // set the material
        mat(material);

        // set the position vertices
        P.clear();
        P.resize(3);
        P[0] = SLVec3f(0,0,0);
        P[1] = SLVec3f(size,0,0);
        P[2] = SLVec3f(0,size,0);

        // define the indices for the used positions
        I16.clear();
        I16.resize(3);
        I16[0] = 0;
        I16[1] = 1;
        I16[2] = 2;
    }
};

void SLSceneBuilderTriangle::build(SLScene *s, SLSceneView *sv, SLSceneID sceneID)
{
    s->name("Triangle Scene Test");
    s->info("Minimal Triangle.");

    // define half the triangle size
    int halfSize = 4;

    // instantiate the triangle
    SLMaterial* m1 ;
    if ( sceneID == SID_Minimal ){
        m1 = new SLMaterial(SLCol4f::BLUE * 0.5f, "myColor");
    } else {
        m1 = new SLMaterial(SLCol4f::YELLOW * 0.5f, "myColor");
    }
    SLMesh* myMesh = new MyTriangle(halfSize*2, "my mesh",m1);

    // create a minimal scene:
    SLNode* scene = new SLNode("triangle scene");
    SLLightSpot* light1 = new SLLightSpot(0.1f);
    light1->translation(halfSize,halfSize,10);
    light1->name("light node");
    scene->addChild(light1);
    SLNode* myNode = new SLNode(myMesh,"my node");
    scene->addChild(myNode);
    sv->sceneViewCamera()->background().colors(SLCol4f(1.0f,0.2f,0.5f),SLCol4f(0.2f,0.2f,0.2f));
    s->root3D(scene);
}
