#include "Gui/WordColorsWindow.h"

namespace GUI
{
    /// Updates and renders the window if it's open.
    /// @param[in,out]  colors_by_word - The colors associated with words to update and render.
    void WordColorsWindow::UpdateAndRender(std::map<std::string, ImVec4>& colors_by_word)
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return;
        }

        /// @todo   Highlight color!

        if (ImGui::Begin("Word Colors", &Open))
        {
            for (auto& word_and_color : colors_by_word)
            {
                float color_components[3] =
                {
                    word_and_color.second.x,
                    word_and_color.second.y,
                    word_and_color.second.z,
                };
                
                ImGui::ColorEdit3(
                    word_and_color.first.c_str(), 
                    color_components,
                    ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);

                word_and_color.second.x = color_components[0];
                word_and_color.second.y = color_components[1];
                word_and_color.second.z = color_components[2];

                ImGui::SameLine();
                ImGui::Text(word_and_color.first.c_str());
            }
        }
        ImGui::End();
        
    }
}
