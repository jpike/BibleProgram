#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_set>
#include <ThirdParty/imgui/imgui.h>
#include <ThirdParty/imgui/imgui_internal.h>
#include "Debugging/Timer.h"
#include "Gui/Gui.h"

namespace GUI
{
    /// Updates and renders a single frame of the GUI.
    void Gui::UpdateAndRender()
    {
        // SELECT THE FIRST TRANSLATION BY DEFAULT.
        bool any_translation_selected = !UserSettings.PrimaryBibleTranslationName.empty();
        if (!any_translation_selected)
        {
            if (!Bible->TranslationsByName.empty())
            {
                UserSettings.SetPrimaryBibleTranslation(Bible->TranslationsByName.begin()->first);
            }
        }

        // RENDER THE MAIN MENU BAR.
        if (ImGui::BeginMainMenuBar())
        {
            // No shortcut keys are displayed for menu items since ImGui doesn't currently handle them.
            constexpr char* NO_SHORTCUT_KEYS = nullptr;

            // RENDER A MENU FOR SELECTING THE CURRENT TRANSLATION.
            if (ImGui::BeginMenu("Translations"))
            {
                // DISPLAY OPTIONS FOR ALL TRANSLATIONS.
                for (const auto& names_and_translations : Bible->TranslationsByName)
                {
                    // ALLOW THE USED TO SELECT A TRANSLATION.
                    bool is_selected = (names_and_translations.first == UserSettings.PrimaryBibleTranslationName);
                    if (ImGui::MenuItem(names_and_translations.first.c_str(), NO_SHORTCUT_KEYS, is_selected))
                    {
                        UserSettings.SetPrimaryBibleTranslation(names_and_translations.first);
                    }

                    // MAKE SURE THE TRANSLATION'S DISPLAY STATUS IS UPDATED.
                    UserSettings.BibleTranslationDisplayStatusesByName[names_and_translations.first] = is_selected;
                }

                ImGui::EndMenu();
            }

            // RENDER THE MENU FOR THE ACCESSING AVAILABLE WINDOWS.
            if (ImGui::BeginMenu("Windows"))
            {
                ImGui::MenuItem("Bible Books", NO_SHORTCUT_KEYS, &BibleBookWindow.Open);
                ImGui::MenuItem("Bible Verses", NO_SHORTCUT_KEYS, &BibleVersesWindow.Open);
                ImGui::EndMenu();
            }

            // RENDER THE DEBUG MENU.
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::MenuItem("Metrics", NO_SHORTCUT_KEYS, &MetricsWindow.Open);
                ImGui::MenuItem("Style Editor", NO_SHORTCUT_KEYS, &StyleEditorWindow.Open);
                ImGui::MenuItem("Demo", NO_SHORTCUT_KEYS, &DemoWindow.Open);
                ImGui::MenuItem("About", NO_SHORTCUT_KEYS, &AboutWindow.Open);

                ImGui::EndMenu();
            }

            // RENDER A VERSE SEARCH BAR.
            // The longest theoretical verse range would be something like
            // 2 Thessalonians 111:111 - 2 Thessalonians 999:999
            // which is just under 50 characters, so 64 characters should be plenty of room.
            static char verse_range_text[64];
            // The width of the text box has been chosen to be long enough for all expected text.
            ImGui::SetNextItemWidth(512.0f);
            // Search box is configured to only return true when enter is pressed to make it
            // clear when the user has finished entering search text.
            bool verse_search_box_enter_pressed = ImGui::InputTextWithHint(
                "###VerseReferenceSearch", 
                "Enter verse range (ex. Genesis 1:1 - Genesis 2:3)", 
                verse_range_text,
                IM_ARRAYSIZE(verse_range_text),
                ImGuiInputTextFlags_EnterReturnsTrue);
            if (verse_search_box_enter_pressed)
            {
                // PARSE THE VERSE RANGE.
                std::optional<BIBLE_DATA::BibleVerseRange> verse_range = BIBLE_DATA::BibleVerseRange::Parse(verse_range_text);

                // DISPLAY THE VERSES IN THE RANGE IF VALID.
                if (verse_range)
                {
                    BibleVersesWindow.SetVerses(*verse_range);
                    BibleVersesWindow.Open = true;
                }
            }
        }
        ImGui::EndMainMenuBar();
        
        // UPDATE AND RENDER ALL WINDOWS.
        std::optional<BIBLE_DATA::BibleVerseRange> selected_verse_range = BibleBookWindow.UpdateAndRender();
        if (selected_verse_range)
        {
            // DISPLAY THE VERSES FOR THE SELECTED VERSE RANGE.
            BibleVersesWindow.SetVerses(*selected_verse_range);
            BibleVersesWindow.Open = true;
        }

        BibleVersesWindow.UpdateAndRender(UserSettings);

        MetricsWindow.UpdateAndRender();
        StyleEditorWindow.UpdateAndRender();
        DemoWindow.UpdateAndRender();
        AboutWindow.UpdateAndRender();
    }
}
