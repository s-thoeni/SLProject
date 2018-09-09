//#############################################################################
//  File:      NewNodeGui.h
//  Author:    Marcus Hudritsch
//  Date:      Summer 2017
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#ifndef NEWNODEGUI_H
#define NEWNODEGUI_H

#include <stdafx.h> // Must be the 1st include followed by  an empty line

class SLScene;
class SLSceneView;

//-----------------------------------------------------------------------------
//! ImGui UI class for the UI of the demo applications
/* The UI is completely build within this class by calling build function
AppNodeGui::build. This build function is passed in the slCreateSceneView and
it is called in SLSceneView::onPaint in every frame.
The entire UI is configured and built on every frame. That is why it is called
"Im" for immediate. See also the SLGLImGui class to see how it minimaly
integrated in the SLProject.
*/
class AppNodeGui
{
    public:
    static void     build(SLScene* s, SLSceneView* sv);
    static SLstring infoText;
};
//-----------------------------------------------------------------------------
#endif
