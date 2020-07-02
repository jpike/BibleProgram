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

        /// @todo   Show custom content about data sources for Bible verses!
        if (ImGui::Begin("About"))
        {
            ImGui::Text("KJV from http://sacred-texts.com/bib/osrc/index.htm");
            ImGui::Text("Other Translations from https://github.com/gratis-bible/bible");
        }
        ImGui::End();

        // SHOW THE BUILT-IN ABOUT WINDOW.
        ImGui::ShowAboutWindow(&Open);
    }
}
