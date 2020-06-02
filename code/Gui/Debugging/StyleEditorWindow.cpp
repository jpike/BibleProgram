#include <ThirdParty/imgui/imgui.h>
#include "Gui/Debugging/StyleEditorWindow.h"

namespace GUI::DEBUGGING
{
    /// Updates and renders a single frame of the style editor window, if it's open.
    void StyleEditorWindow::UpdateAndRender()
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return;
        }

        // SHOW THE BUILT-IN STYLE EDITOR IN A WINDOW.
        if (ImGui::Begin("Style Editor", &Open))
        {
            ImGui::ShowStyleEditor();
        }
        ImGui::End();
    }
}
