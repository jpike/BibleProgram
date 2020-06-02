#include <ThirdParty/imgui/imgui.h>
#include "Gui/Debugging/AboutWindow.h"

namespace GUI::DEBUGGING
{
    /// Updates and renders a single frame of the about window, if it's open.
    void AboutWindow::UpdateAndRender()
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return;
        }

        // SHOW THE BUILT-IN ABOUT WINDOW.
        ImGui::ShowAboutWindow(&Open);
    }
}
