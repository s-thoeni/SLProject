#include "SLGuiBuilderMinimal.h"

#include <SLApplication.h>
#include <UtilImGUI.h>

onSceneChange fireSceneChange;

/*
 * This GUI Builder builds a minimalistic GUI with ImGui containing two menu items
 * and a box to show the scene's info-text.
 */
void build (SLScene* s, SLSceneView* sv)
{
    SLSceneID sid = SLApplication::sceneID;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Test Scenes"))
        {
            if (ImGui::MenuItem("Minimal Scene", 0, sid==SID_Minimal))
                fireSceneChange(SID_Minimal);
            if (ImGui::MenuItem("Figure Scene", 0, sid==SID_Figure))
                fireSceneChange(SID_Figure);

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // show scene info:

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;
    SLfloat w = (SLfloat)sv->scrW();
    ImVec2 size = ImGui::CalcTextSize(s->info().c_str(), 0, true, w);
    SLfloat h = size.y + SLGLImGui::fontPropDots * 1.2f;
    SLstring info = "Scene Info: " + s->info();

    ImGui::SetNextWindowPos(ImVec2(0,sv->scrH()-h));
    ImGui::SetNextWindowSize(ImVec2(w,h));
    ImGui::Begin("Scene Information", 0, window_flags);
    ImGui::TextWrapped("%s", info.c_str());
    ImGui::End();
}

SLGuiBuilderMinimal::SLGuiBuilderMinimal()
{
    this->buildFunction = (void*) build;    
}

void SLGuiBuilderMinimal::registerSceneChangeListener(onSceneChange listener)
{
    fireSceneChange = listener;
}

void SLGuiBuilderMinimal::onSceneCreated(SLuint dpi)
{
    UtilImGUI::scale(dpi);
}
