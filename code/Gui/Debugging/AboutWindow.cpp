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

        // SHOW THE MAIN ABOUT WINDOW.
        if (ImGui::Begin("About", &Open))
        {
            ImGui::Text("KJV from http://sacred-texts.com/bib/osrc/index.htm");
            ImGui::Text("Other Translations from https://github.com/gratis-bible/bible");

            if (ImGui::Button("About the GUI"))
            {
                GuiSubWindowOpen = true;
            }
        }
        ImGui::End();

        // SHOW THE BUILT-IN GUI ABOUT WINDOW.
        if (GuiSubWindowOpen)
        {
            ImGui::ShowAboutWindow(&GuiSubWindowOpen);
        }
    }
}
