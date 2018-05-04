#include "SLWindowAndroid.h"

#include <SLInterface.h>
#include <SLScene.h>

int SLWindowAndroid::abstractShow()
{

    ////////////////////////////////////////////////////
    slCreateAppAndScene(  this->devicePath + "/shaders/",
                          this->devicePath + "/models/",
                          this->devicePath + "/textures/",
                          this->devicePath + "/videos/",
                          this->devicePath + "/fonts/",
                          this->devicePath + "/calibrations/",
                          this->devicePath + "/config/",
                          "AppDemoAndroid"
                          );
    ////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////
    this->svIndex = slCreateSceneView((int) this->width,
                                (int) this->height,
                                (int) this->dpi,
                                SID_Revolver,
                                (void *) this->onWndUpdateCallback,
                                0,
                                0,
                                this->guiBuilder->buildFunction);
    ////////////////////////////////////////////////////////////////////
    this->onSceneCreated();

    return 0;
}
