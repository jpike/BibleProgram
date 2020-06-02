#include <ThirdParty/imgui/imgui.h>
#include "Gui/Debugging/GuiMetricsWindow.h"

namespace GUI::DEBUGGING
{
    /// Updates and renders a single frame of the GUI metrics window, if it's open.
    void GuiMetricsWindow::UpdateAndRender()
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return;
        }

        // SHOW THE BUILT-IN METRICS WINDOW.
        ImGui::ShowMetricsWindow(&Open);
    }
}
