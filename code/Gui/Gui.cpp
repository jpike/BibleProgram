#include <iostream>
#include <ThirdParty/imgui/imgui.h>
#include <ThirdParty/imgui/imgui_internal.h>
#include "Debugging/Timer.h"
#include "Gui/Gui.h"

namespace GUI
{
    /// Updates and renders a single frame of the GUI.
    /// @param[in]  bible - The Bible to display in the window.
    void Gui::UpdateAndRender(const BIBLE_DATA::Bible& bible)
    {
        // RENDER THE MAIN MENU BAR.
        if (ImGui::BeginMainMenuBar())
        {
            // No shortcut keys are displayed for menu items since ImGui doesn't currently handle them.
            constexpr char* NO_SHORTCUT_KEYS = nullptr;

            // RENDER THE MENU FOR THE ACCESSING AVAILABLE WINDOWS.
            if (ImGui::BeginMenu("Windows"))
            {
                ImGui::MenuItem("Bible Books", NO_SHORTCUT_KEYS, &BibleBookWindow.Open);

                ImGui::EndMenu();
            }

            // RENDER THE DEBUG MENU.
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::MenuItem("Metrics", NO_SHORTCUT_KEYS, &MetricsWindow.Open);
                ImGui::MenuItem("Style Editor", NO_SHORTCUT_KEYS, &StyleEditorWindow.Open);
                ImGui::MenuItem("About", NO_SHORTCUT_KEYS, &AboutWindow.Open);

                ImGui::EndMenu();
            }

            // RENDER A VERSE SEARCH BAR.
            /// @todo   Seems to return true either if enter pressed or value changed?
            static char verse_range_text[64];
            ImGui::SetNextItemWidth(256.0f);
            bool verse_search_box_enter_pressed = ImGui::InputTextWithHint(
                "###VerseReferenceSearch", 
                "Enter book/chapter/verse range", 
                verse_range_text,
                IM_ARRAYSIZE(verse_range_text),
                ImGuiInputTextFlags_EnterReturnsTrue);
            if (verse_search_box_enter_pressed)
            {
                std::optional<std::pair<BIBLE_DATA::BibleVerseId, BIBLE_DATA::BibleVerseId>> verse_range = BIBLE_DATA::BibleVerseId::ParseRange(verse_range_text);
                if (verse_range)
                {
                    BibleVersesWindow.StartingVerseId = verse_range->first;
                    BibleVersesWindow.EndingVerseId = verse_range->second;
                    BibleVersesWindow.Verses = bible.GetVerses(verse_range->first, verse_range->second);

                    /// @todo   Get verses and render!
                    BibleVersesWindow.Open = true;
                }
            }

            // RENDER A WORD SEARCH BAR.
            /// @todo   This is a bad idea in the naive way - requires about 12 GB of memory.
#if 0
            static char word_search_text[64];
            ImGui::SetNextItemWidth(256.0f);
            bool word_search_box_enter_pressed = ImGui::InputTextWithHint(
                "Search##WordSearch",
                "Enter word",
                word_search_text,
                IM_ARRAYSIZE(word_search_text),
                ImGuiInputTextFlags_EnterReturnsTrue);
            if (word_search_box_enter_pressed)
            {
                BibleVersesWindow.Verses = bible.GetVerses(word_search_text);
                if (!BibleVersesWindow.Verses.empty())
                {
                    BibleVersesWindow.StartingVerseId = BibleVersesWindow.Verses.front().Id;
                    BibleVersesWindow.EndingVerseId = BibleVersesWindow.Verses.back().Id;
                    BibleVersesWindow.Open = true;
                }
            }
#endif
        }
        ImGui::EndMainMenuBar();

        // UPDATE AND RENDER ALL WINDOWS.
        const BIBLE_DATA::BibleChapter* selected_chapter = BibleBookWindow.UpdateAndRender(bible);
        if (selected_chapter)
        {
            BIBLE_DATA::BibleVerseId chapter_starting_verse_id =
            {
                .Book = selected_chapter->Book,
                .ChapterNumber = selected_chapter->Number,
                .VerseNumber = 1
            };
            auto chapter_starting_verse = bible.TranslationsByName.at("KJV").VersesById.lower_bound(chapter_starting_verse_id);
            BIBLE_DATA::BibleVerseId chapter_ending_verse_id =
            {
                .Book = selected_chapter->Book,
                .ChapterNumber = selected_chapter->Number,
                .VerseNumber = static_cast<unsigned int>(selected_chapter->VersesByNumber.size())
            };

            BibleVersesWindow.StartingVerseId = chapter_starting_verse_id;
            BibleVersesWindow.EndingVerseId = chapter_ending_verse_id;
            BibleVersesWindow.Verses = bible.GetVerses(chapter_starting_verse_id, chapter_ending_verse_id);

            /// @todo   Get verses and render!
            BibleVersesWindow.Open = true;
        }

        BibleVersesWindow.UpdateAndRender();

        MetricsWindow.UpdateAndRender();
        StyleEditorWindow.UpdateAndRender();
        AboutWindow.UpdateAndRender();
    }

#if 0
    /// Updates and renders a single frame of the GUI.
    void Gui::UpdateAndRender()
    {
        DEBUGGING::Timer function_timer("UpdateAndRender()");

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Translations"))
            {
                if (ImGui::BeginCombo("Primary Translation", TranslationConfig.CurrentTranslationName.c_str()))
                {
                    for (const auto& translation_name : TranslationConfig.AvailableTranslationNames)
                    {
                        bool is_selected = (translation_name == TranslationConfig.CurrentTranslationName);
                        if (ImGui::Selectable(translation_name.c_str(), is_selected))
                        {
                            TranslationConfig.CurrentTranslationName = translation_name;
                        }
                    }
                    ImGui::EndCombo();
                }
                             
                for (const auto& translation_name : TranslationConfig.AvailableTranslationNames)
                {
                    bool translation_enabled = TranslationConfig.EnabledStatusByTranslationName[translation_name];
                    if (ImGui::MenuItem(translation_name.c_str(), NULL, translation_enabled))
                    {
                        TranslationConfig.EnabledStatusByTranslationName[translation_name] = !translation_enabled;
                    }
                }

                ImGui::EndMenu();
            }

            DebugGui.UpdateAndRenderMenu();

            ImGui::EndMainMenuBar();
        }

        DebugGui.UpdateAndRender();

        bool chapter_just_selected = false;
        ImGui::Begin("Bible Window");
        for (const auto& id_and_book : Bible.BooksById)
        {
            std::string book_name = BIBLE_DATA::BibleBook::FullName(id_and_book.first);
            if (ImGui::TreeNodeEx(book_name.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
            {
                const BIBLE_DATA::BibleBook& book = id_and_book.second;
                for (const auto& number_and_chapter : book.ChaptersByNumber)
                {
                    std::string chapter_text = "Chapter " + std::to_string(number_and_chapter.first);
                    if (ImGui::Selectable(chapter_text.c_str()))
                    {
                        chapter_just_selected = (
                            !SelectedChapter ||
                            (&number_and_chapter.second != SelectedChapter));

                        SelectedChapter = &number_and_chapter.second;
                    }
                }
            }
        }
        ImGui::End();

        if (SelectedChapter)
        {
            ImGui::Begin("Chapter Window");

            DEBUGGING::Timer translation_enabled_timer("Translation enabled check");
            unsigned int enabled_translation_count = 0;
            for (const auto& translation_name : TranslationConfig.AvailableTranslationNames)
            {
                bool translation_enabled = TranslationConfig.EnabledStatusByTranslationName[translation_name];
                if (translation_enabled)
                {
                    ++enabled_translation_count;
                }
            }

            ImGui::Columns(enabled_translation_count, "Translation Columns");

            for (const auto& translation_name : TranslationConfig.AvailableTranslationNames)
            {
                bool translation_enabled = TranslationConfig.EnabledStatusByTranslationName[translation_name];
                if (translation_enabled)
                {
                    ImGui::Text(translation_name.c_str());
                    ImGui::NextColumn();
                }
            }

            std::cout << translation_enabled_timer.GetElapsedTimeText() << std::endl;

            ImGui::Separator();

            for (const auto& translation_name : TranslationConfig.AvailableTranslationNames)
            {
                bool translation_enabled = TranslationConfig.EnabledStatusByTranslationName[translation_name];
                if (!translation_enabled)
                {
                    continue;
                }

                if (chapter_just_selected)
                {
                    DEBUGGING::Timer verse_range_timer("Verse range timer");
                    auto current_translation = Bible.TranslationsByName[translation_name];
                    BIBLE_DATA::BibleVerseId chapter_starting_verse_id =
                    {
                        .Book = SelectedChapter->Book,
                        .ChapterNumber = SelectedChapter->Number,
                        .VerseNumber = 1
                    };
                    auto chapter_starting_verse = current_translation.VersesById.lower_bound(chapter_starting_verse_id);
                    BIBLE_DATA::BibleVerseId chapter_ending_verse_id =
                    {
                        .Book = SelectedChapter->Book,
                        .ChapterNumber = SelectedChapter->Number,
                        .VerseNumber = static_cast<unsigned int>(SelectedChapter->VersesByNumber.size())
                    };
                    auto chapter_after_ending_verse = current_translation.VersesById.upper_bound(chapter_ending_verse_id);

                    CurrentVersesByTranslationName[translation_name].clear();
                    for (auto& id_and_verse = chapter_starting_verse; id_and_verse != chapter_after_ending_verse; ++id_and_verse)
                    {
                        CurrentVersesByTranslationName[translation_name].push_back(id_and_verse->second);
                    }

                    std::cout << verse_range_timer.GetElapsedTimeText() << std::endl;
                }
                
                DEBUGGING::Timer verse_render_timer("Verse render timer");
                for (auto& verse : CurrentVersesByTranslationName[translation_name])
                {
                    /// @todo   Render same verse across all translations on same horizontal line?
                    Render(verse);
                }
                std::cout << verse_render_timer.GetElapsedTimeText() << std::endl;

                ImGui::NextColumn();
                // Note that something about the way we're rendering verse text
                // prevents the cursor positioning (particularly the y position)
                // from being properly reset.  The below sort of works, but it's
                // not perfect and will need to be adjusted later.
                ImGuiWindow* window = ImGui::GetCurrentWindow();
                window->DC.CursorPosPrevLine.x = window->DC.CursorPos.x;
                window->DC.CursorPosPrevLine.y = window->DC.CursorPos.y;
            }

            ImGui::Columns(1);

            ImGui::End();
        }

        std::cout << function_timer.GetElapsedTimeText() << std::endl;
    }

    void Gui::Render(BIBLE_DATA::BibleVerse& verse)
    {
        // MAIN LOGIC.
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec4 color;
        color.w = 1.0f;
        const std::vector<BIBLE_DATA::Token>* verse_tokens = verse.GetTokens();
        for (std::size_t token_index = 0; token_index < verse_tokens->size(); ++token_index)
        {
            const auto& token = verse_tokens->at(token_index);
            if (token.Type == BIBLE_DATA::TokenType::SPACE)
            {
                continue;
            }

            ImGuiContext& gui_context = *GImGui;
            int text_length = ImFormatString(gui_context.TempBuffer, IM_ARRAYSIZE(gui_context.TempBuffer), token.Text.c_str());
            const char* text_begin = gui_context.TempBuffer;
            const char* text_end = text_begin + text_length;

            /// @todo   The code here is very hacky...Need to spend more time refining it.
            ImGui::SameLine();
            ImVec2 text_position(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
            constexpr float WRAP_TO_END_OF_WINDOW = 0.0f;
            float wrap_width = ImGui::CalcWrapWidthForPos(window->DC.CursorPos, WRAP_TO_END_OF_WINDOW);
            ImVec2 text_size = ImGui::CalcTextSize(text_begin, text_end, false, wrap_width);
            ImRect text_bounding_box(text_position, text_position + text_size);

            // Accounting for the spacing between items is very important for proper wrapping.
            float text_width = text_bounding_box.GetWidth() + gui_context.Style.ItemSpacing.x;
            bool current_text_can_fit_on_line = (text_width < wrap_width);
            if (!current_text_can_fit_on_line)
            {
                // START A NEW LINE.
                ImGui::NewLine();

                // RECALCULATE THE BOUNDING BOX FOR THE TEXT FOR THE NEW LINE.
                text_position = ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + window->DC.CurrLineTextBaseOffset);
                wrap_width = ImGui::CalcWrapWidthForPos(window->DC.CursorPos, WRAP_TO_END_OF_WINDOW);
                text_size = ImGui::CalcTextSize(text_begin, text_end, false, wrap_width);
                text_bounding_box = ImRect(text_position, text_position + text_size);
            }

            ImGui::ItemSize(text_size);
            ImGui::ItemAdd(text_bounding_box, 0);

            // SET THE TEXT COLOR.
            // A temporarily different (usually) highlight color is used when hovering over.
            if (ImGui::IsItemHovered())
            {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            }
            else
            {
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            }
            // Render (we don't hide text after ## in this end-user function)
            // RenderTextWrapped didn't work, so we're rendering text directly.
            ImGui::RenderText(text_bounding_box.Min, text_begin, text_end);
            ImGui::PopStyleColor();
            // ADJUST THE COLOR FOR TESTING.
            // This is just temporary and will be replaced with fixed lookups/customizable options later.
            if (color.x <= color.y)
            {
                color.x += 0.1f;
            }
            else if (color.y <= color.z)
            {
                color.y += 0.1f;
            }
            else if (color.z <= color.x)
            {
                color.z += 0.1f;
            }
        }
    }
#endif
}
