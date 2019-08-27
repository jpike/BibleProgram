#include <ThirdParty/imgui/imgui.h>
#include "Gui/Gui.h"

namespace GUI
{
    /// Updates and renders a single frame of the GUI.
    void Gui::UpdateAndRender()
    {
        static bool show_metrics = false;
        static bool show_style_editor = false;
        static bool show_about = false;

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::MenuItem("Metrics", nullptr, &show_metrics);
                ImGui::MenuItem("Style Editor", nullptr, &show_style_editor);
                ImGui::MenuItem("About", nullptr, &show_about);

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (show_metrics)
        {
            ImGui::ShowMetricsWindow(&show_metrics);
        }

        if (show_style_editor)
        {
            ImGui::Begin("Style Editor", &show_style_editor);
            ImGui::ShowStyleEditor();
            ImGui::End();
        }

        if (show_about)
        {
            ImGui::ShowAboutWindow(&show_about);
        }

        static const BIBLE_DATA::BibleChapter* selected_chapter = nullptr;
        ImGui::Begin("Bible Window");
        for (const auto& book : Bible.Books)
        {
            if (ImGui::TreeNodeEx(book.Name.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
            {
                for (const auto& chapter : book.Chapters)
                {
                    std::string chapter_text = "Chapter " + std::to_string(chapter.Number);
                    if (ImGui::Selectable(chapter_text.c_str()))
                    {
                        selected_chapter = &chapter;
                    }
                }
            }
        }
        ImGui::End();

        if (selected_chapter)
        {
            ImGui::Begin("Chapter Window");
            for (const auto& verse : selected_chapter->Verses)
            {
                std::string verse_number_text = std::to_string(verse.VerseNumber);
                //ImGui::InputTextMultiline(verse_number_text.c_str(), const_cast<char *>(verse.Text.c_str()), verse.Text.length(), ImVec2(), ImGuiInputTextFlags_ReadOnly);
                ImGui::TextWrapped(verse.Text.c_str());
            }
            ImGui::End();
        }
    }
}
