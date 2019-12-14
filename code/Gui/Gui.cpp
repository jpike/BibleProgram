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

        static bool translation_mapped_initialized = false;
        static std::map<const char*, bool> translation_enabled_status_by_name;

        static std::vector<const char*> available_translation_names;

        if (!translation_mapped_initialized)
        {
            for (const auto& name_and_translation : Bible.TranslationsByName)
            {
                const char* translation_name = name_and_translation.first.c_str();
                available_translation_names.push_back(translation_name);

                /// @todo   Figure out when to initialize this.
                bool translation_status_exists = translation_enabled_status_by_name.contains(translation_name);
                if (!translation_status_exists)
                {
                    translation_enabled_status_by_name[translation_name] = true;
                }
            }
        }
        static const char* current_translation_name = available_translation_names[0];
        translation_mapped_initialized = true;

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Translations"))
            {
                if (ImGui::BeginCombo("Primary Translation", current_translation_name))
                {
                    for (const auto& translation_name : available_translation_names)
                    {
                        bool is_selected = (translation_name == current_translation_name);
                        if (ImGui::Selectable(translation_name, is_selected))
                        {
                            current_translation_name = translation_name;
                        }
                    }
                    ImGui::EndCombo();
                }
             
                
                for (const auto& translation_name : available_translation_names)
                {
                    bool translation_enabled = translation_enabled_status_by_name[translation_name];
                    if (ImGui::MenuItem(translation_name, NULL, translation_enabled))
                    {
                        translation_enabled_status_by_name[translation_name] = !translation_enabled;
                    }
                }

                ImGui::EndMenu();
            }

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

            unsigned int enabled_translation_count = 0;
            for (const auto& translation_name : available_translation_names)
            {
                bool translation_enabled = translation_enabled_status_by_name[translation_name];
                if (translation_enabled)
                {
                    ++enabled_translation_count;
                }
            }

            ImGui::Columns(enabled_translation_count, "Translation Columns");

            for (const auto& translation_name : available_translation_names)
            {
                bool translation_enabled = translation_enabled_status_by_name[translation_name];
                if (translation_enabled)
                {
                    ImGui::Text(translation_name);
                    ImGui::NextColumn();
                }
            }

            ImGui::Separator();

            for (const auto& translation_name : available_translation_names)
            {
                bool translation_enabled = translation_enabled_status_by_name[translation_name];
                if (!translation_enabled)
                {
                    continue;
                }

                const auto current_translation = Bible.TranslationsByName[translation_name];

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
                    /// @todo   Render same verse across all translations on same horizontal line?
                    const BIBLE_DATA::BibleVerse& verse = id_and_verse->second;
                    Render(verse);
                    /*
                    std::string verse_number_text = std::to_string(verse.Id.VerseNumber);
                    //ImGui::InputTextMultiline(verse_number_text.c_str(), const_cast<char *>(verse.Text.c_str()), verse.Text.length(), ImVec2(), ImGuiInputTextFlags_ReadOnly);

                    std::string verse_number_and_content_text = verse_number_text + ": " + verse.Text;

                    ImGui::TextWrapped(verse_number_and_content_text.c_str());
                    */
                }

                /*std::string chapter_text;
                for (auto id_and_verse = chapter_starting_verse; id_and_verse != chapter_after_ending_verse; ++id_and_verse)
                {
                    const BIBLE_DATA::BibleVerse& verse = id_and_verse->second;
                    chapter_text += " " + verse.Text;
                }

                ImGui::InputTextMultiline(
                    ("##ChapterVerses" + std::string(translation_name)).c_str(),
                    // Need to pass pointer to non-const.
                    const_cast<char*>(chapter_text.c_str()),
                    chapter_text.size(),
                    //ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16),
                    ImVec2(),
                    ImGuiInputTextFlags_ReadOnly);*/

                // https://github.com/ocornut/imgui/issues/952
                // https://github.com/ocornut/imgui/issues/902
                // https://github.com/ocornut/imgui/issues/986
                // https://github.com/ocornut/imgui/issues/200
                // https://github.com/ocornut/imgui/issues/2728
                // https://github.com/ocornut/imgui/issues/950
                // https://github.com/BalazsJako/ImGuiColorTextEdit

                ImGui::NextColumn();
            }

            ImGui::Columns(1);

            ImGui::End();
        }
    }

    void Gui::Render(const BIBLE_DATA::BibleVerse& verse)
    {
        std::string verse_number = std::to_string(verse.Id.VerseNumber);
        // This font scaling stuff technically works, but doesn't look that great, so it's commented out for now.
        //ImGui::SetWindowFontScale(0.8f);
        ImGui::TextDisabled(verse_number.c_str());
        ImGui::SameLine();
        //ImGui::SetWindowFontScale(1.0f);
        ImGui::TextWrapped(verse.Text.c_str());
    }
}
