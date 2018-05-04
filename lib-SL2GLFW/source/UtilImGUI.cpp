#include "UtilImGUI.h"

#include <SLApplication.h>
#include <SLGLImGui.h>

void UtilImGUI::scale(SLuint dotsPerInch)
{
    // Scale for proportioanl and fixed size fonts
    SLfloat dpiScaleProp = dotsPerInch / 120.0f;
    SLfloat dpiScaleFixed = dotsPerInch / 142.0f;

    // Default settings for the first time
    SLGLImGui::fontPropDots  = SL_max(16.0f * dpiScaleProp, 16.0f);
    SLGLImGui::fontFixedDots = SL_max(13.0f * dpiScaleFixed, 13.0f);

    // Adjust UI paddings on DPI
    ImGuiStyle& style = ImGui::GetStyle();
    style.FramePadding.x = SL_max(8.0f * dpiScaleFixed, 8.0f);
    style.WindowPadding.x = style.FramePadding.x;
    style.FramePadding.y = SL_max(3.0f * dpiScaleFixed, 3.0f);
    style.ItemSpacing.x = SL_max(8.0f * dpiScaleFixed, 8.0f);
    style.ItemSpacing.y = SL_max(3.0f * dpiScaleFixed, 3.0f);
    style.ItemInnerSpacing.x = style.ItemSpacing.y;
}
