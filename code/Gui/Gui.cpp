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
        for (const auto& id_and_book : Bible.BooksById)
        {
            std::string book_name = BIBLE_DATA::BibleBook::FullName(id_and_book.first);
            if (ImGui::TreeNodeEx(book_name.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
            {
                const BIBLE_DATA::BibleBook& book = id_and_book.second;
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
            const auto current_translation = Bible.TranslationsByName["KJV"];
            BIBLE_DATA::BibleVerseId chapter_starting_verse_id =
            {
                .Book = selected_chapter->Book,
                .ChapterNumber = selected_chapter->Number,
                .VerseNumber = 1
            };
            const auto chapter_starting_verse = current_translation.VersesById.lower_bound(chapter_starting_verse_id);
            BIBLE_DATA::BibleVerseId chapter_ending_verse_id =
            {
                .Book = selected_chapter->Book,
                .ChapterNumber = selected_chapter->Number,
                .VerseNumber = selected_chapter->VerseCount
            };
            const auto chapter_after_ending_verse = current_translation.VersesById.upper_bound(chapter_ending_verse_id);
            for (auto id_and_verse = chapter_starting_verse; id_and_verse != chapter_after_ending_verse; ++id_and_verse)
            {
                const BIBLE_DATA::BibleVerse& verse = id_and_verse->second;
                std::string verse_number_text = std::to_string(verse.Id.VerseNumber);
                //ImGui::InputTextMultiline(verse_number_text.c_str(), const_cast<char *>(verse.Text.c_str()), verse.Text.length(), ImVec2(), ImGuiInputTextFlags_ReadOnly);
                ImGui::TextWrapped(verse.Text.c_str());
            }
            ImGui::End();
        }
    }
}
