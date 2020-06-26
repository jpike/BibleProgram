#include <ThirdParty/imgui/imgui.h>
#include "Gui/Debugging/DemoWindow.h"

namespace GUI::DEBUGGING
{
    /// Updates and renders a single frame of the GUI demo window, if it's open.
    void DemoWindow::UpdateAndRender()
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return;
        }

        // SHOW THE BUILT-IN DEMO WINDOW.
        ImGui::ShowDemoWindow(&Open);
    }
}
