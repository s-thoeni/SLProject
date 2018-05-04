#include "SLSceneBuilderMinimal.h"

#include <SLLightSpot.h>
#include <SLLight.h>
#include <SLRectangle.h>
#include <SLCoordAxis.h>

/*
 * This minimalistic scene loader builds a scene by a given SLSceneID.
 * If the SLSceneId is not supported, an empty scene will be built.
 *
 * It only supports the SID_MINIMAL.
 *
 */

void SLSceneBuilderMinimal::build(SLScene* s, SLSceneView* sv, SLSceneID sceneID)
{
    if (sceneID == SID_Minimal) //...................................................
    {
        // Set scene name and info string
        s->name("Minimal Scene Test");
        s->info("Minimal texture mapping example with one light source.");

        // Create textures and materials
        SLGLTexture* texC = new SLGLTexture("earth1024_C.jpg");
        SLMaterial* m1 = new SLMaterial("m1", texC);

        // Create a scene group node
        SLNode* scene = new SLNode("scene node");

        // Create a light source node
        SLLightSpot* light1 = new SLLightSpot(0.3f);
        light1->translation(0,0,5);
        light1->lookAt(0,0,0);
        light1->name("light node");
        scene->addChild(light1);

        // Create meshes and nodes
        SLMesh* rectMesh = new SLRectangle(SLVec2f(-5,-5),SLVec2f(5,5),1,1,"rectangle mesh",m1);
        SLNode* rectNode = new SLNode(rectMesh,"rectangle node");
        scene->addChild(rectNode);

        SLNode* axisNode = new SLNode(new SLCoordAxis(),"axis node");
        scene->addChild(axisNode);

        // Set background color and the root scene node
        sv->sceneViewCamera()->background().colors(SLCol4f(0.7f,0.7f,0.7f),SLCol4f(0.2f,0.2f,0.2f));

        // pass the scene group as root node
        s->root3D(scene);

        // Save energy
        sv->doWaitOnIdle(true);
    } else {

        // Set scene name and info string
        s->name("Scene not found!");
        s->info("This scene is not available!");

        sv->doWaitOnIdle(true);
    }


}
