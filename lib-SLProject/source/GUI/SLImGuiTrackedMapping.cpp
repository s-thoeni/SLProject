//#############################################################################
//  File:      SLImGuiTrackedMapping.cpp
//  Author:    Michael Goettlicher
//  Date:      April 2018
//  Codestyle: https://github.com/cpvrlab/SLProject/wiki/Coding-Style-Guidelines
//  Copyright: Marcus Hudritsch
//             This software is provide under the GNU General Public License
//             Please visit: http://opensource.org/licenses/GPL-3.0
//#############################################################################

#include <stdafx.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <SLImGuiTrackedMapping.h>
#include <SLCVTrackedMapping.h>

//-----------------------------------------------------------------------------
SLImGuiTrackedMapping::SLImGuiTrackedMapping(string name, SLCVTrackedMapping* mappingTracker)
    : SLImGuiInfosDialog(name),
    _mappingTracker(mappingTracker)
{
}
//-----------------------------------------------------------------------------
void SLImGuiTrackedMapping::buildInfos()
{
    if (ImGui::Button("Reset", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f))) {
        _mappingTracker->Reset();
        _mappingTracker->setState(SLCVTrackedMapping::INITIALIZE);
    }

    //add tracking state
    ImGui::Text("Tracking State : %s ", _mappingTracker->getPrintableState().c_str());
    //add number of matches map points in current frame
    ImGui::Text("Num Map Matches: %d ", _mappingTracker->getNMapMatches());
    //number of map points
    ImGui::Text("Num Map Pts: %d ", _mappingTracker->mapPointsCount());
    //add number of keyframes
    ImGui::Text("Number of Keyframes : %d ", _mappingTracker->getNumKeyFrames());
    //add loop closings counter
    ImGui::Text("Number of LoopClosings : %d ", _mappingTracker->getNumLoopClosings());


    //else if (ImGui::Button("Track VO", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f))) {
    //    _mappingTracker->setState(SLCVTrackedMapping::TRACK_VO);
    //}
    //else if (ImGui::Button("Track 3D Pts", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f))) {
    //    _mappingTracker->setState(SLCVTrackedMapping::TRACK_3DPTS);
    //}
    //else if (ImGui::Button("Track optical flow", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f))) {
    //    _mappingTracker->setState(SLCVTrackedMapping::TRACK_OPTICAL_FLOW);
    //}

#ifdef ANDROID
    float bHeigth = 200.0f;
#else
    float bHeigth = 60.0f;
#endif

    if (ImGui::Button("Add key frame", ImVec2(ImGui::GetContentRegionAvailWidth(), bHeigth))) {
        _mappingTracker->mapNextFrame();
    }
    else if (ImGui::Button("Save map", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.0f))) {
        _mappingTracker->saveMap();
    }
}