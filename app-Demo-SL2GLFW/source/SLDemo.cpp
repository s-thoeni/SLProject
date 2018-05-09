//#############################################################################
//  File:      MinimalDemo.cpp
//  Author:    Benjamin Fankhauser
//  Date:      May 2018
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <stdafx.h>           // precompiled headers
#ifdef SL_MEMLEAKDETECT       // set in SL.h for debug config only
#include <debug_new.h>        // memory leak detector
#endif

#include <thread>

#include <SLWindowGlfw.h>
#include <SLApplication.h>
#include <SLScene.h>
#include <SLSceneView.h>

#include "SLGuiBuilderDemo.h"
#include "SLGuiBuilderMinimal.h"
#include "SLSceneBuilderDemo.h"
#include "SLSceneBuilderMinimal.h"
#include "SLSceneBuilderTriangle.h"

int counter (0);
std::string myTemplate = "helloGLFW";
std::string result;

class SLResourceLoaderGlfw : public SLResourceLoader
{
public:

    SLbyte* getResource()
    {
        result = myTemplate + std::to_string(++counter);
        std::this_thread::sleep_for(chrono::seconds(2));
        return (SLbyte*) result.c_str();
    }

};

int main(int argc, char* argv[])
{
    SLWindowGlfw* window = new SLWindowGlfw((SLSceneID) SL_STARTSCENE, 800);

    window->guiBuilder = new SLGuiBuilderMinimal();
//    window->sceneBuilder = new SLSceneBuilderMinimal();
    window->sceneBuilder = new SLSceneBuilderTriangle();
    window->sceneBuilder->resourceLoader = new SLResourceLoaderGlfw();

//    window->guiBuilder = new SLGuiBuilderDemo();
//    window->sceneBuilder = new SLSceneBuilderDemo();

    window->show();

    delete window;
}
//-----------------------------------------------------------------------------

