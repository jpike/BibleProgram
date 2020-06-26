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
    /// @param[in]  bible - The Bible to display in the window.
    void Gui::UpdateAndRender(const BIBLE_DATA::Bible& bible)
    {
        // SELECT THE FIRST TRANSLATION BY DEFAULT.
        bool any_translation_selected = !CurrentTranslationName.empty();
        if (!any_translation_selected)
        {
            if (!bible.TranslationsByName.empty())
            {
                CurrentTranslationName = bible.TranslationsByName.begin()->first;
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
                for (const auto& names_and_translations : bible.TranslationsByName)
                {
                    // ALLOW THE USED TO SELECT A TRANSLATION.
                    bool is_selected = (names_and_translations.first == CurrentTranslationName);
                    if (ImGui::MenuItem(names_and_translations.first.c_str(), NO_SHORTCUT_KEYS, is_selected))
                    {
                        CurrentTranslationName = names_and_translations.first;

                        if (BibleVersesWindow.Open)
                        {
                            std::shared_ptr<BIBLE_DATA::BibleTranslation> current_translation = bible.TranslationsByName.at(CurrentTranslationName);
                            std::vector<BIBLE_DATA::BibleVerse> verses = current_translation->GetVerses(BibleVersesWindow.VerseRange);
                            BibleVersesWindow.SetVerses(BibleVersesWindow.VerseRange, verses);
                        }
                    }
                }

                ImGui::EndMenu();
            }

            // RENDER THE MENU FOR THE ACCESSING AVAILABLE WINDOWS.
            if (ImGui::BeginMenu("Windows"))
            {
                ImGui::MenuItem("Bible Books", NO_SHORTCUT_KEYS, &BibleBookWindow.Open);
                ImGui::MenuItem("Word Statistics", NO_SHORTCUT_KEYS, &WordStatisticsWindow.Open);
                if (ImGui::MenuItem("Word Colors", NO_SHORTCUT_KEYS, &WordColorsWindow.Open))
                {
                    if (ColorsByWord.empty())
                    {
                        UpdateColorLookup(bible);
                    }
                }

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
                std::optional<BIBLE_DATA::BibleVerseRange> verse_range = BIBLE_DATA::BibleVerseRange::Parse(verse_range_text);
                if (verse_range)
                {
                    if (!CurrentTranslationName.empty())
                    {
                        std::shared_ptr<BIBLE_DATA::BibleTranslation> current_translation = bible.TranslationsByName.at(CurrentTranslationName);
                        std::vector<BIBLE_DATA::BibleVerse> verses = current_translation->GetVerses(*verse_range);
                        BibleVersesWindow.SetVerses(*verse_range, verses);
                        BibleVersesWindow.Open = true;
                    }
                }
            }

            // RENDER A WORD SEARCH BAR.
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
#if 0
                /// @todo   This is a bad idea in the naive way - requires about 12 GB of memory.
                BibleVersesWindow.Verses = bible.GetVerses(word_search_text);
                if (!BibleVersesWindow.Verses.empty())
                {
                    BibleVersesWindow.StartingVerseId = BibleVersesWindow.Verses.front().Id;
                    BibleVersesWindow.EndingVerseId = BibleVersesWindow.Verses.back().Id;
                    BibleVersesWindow.Open = true;
                }
#endif
            }
        }
        ImGui::EndMainMenuBar();
        
        // UPDATE AND RENDER ALL WINDOWS.
        std::optional<BIBLE_DATA::BibleVerseRange> selected_verse_range = BibleBookWindow.UpdateAndRender();
        if (selected_verse_range)
        {
            if (!CurrentTranslationName.empty())
            {
                std::shared_ptr<BIBLE_DATA::BibleTranslation> current_translation = bible.TranslationsByName.at(CurrentTranslationName);
                std::vector<BIBLE_DATA::BibleVerse> verses = current_translation->GetVerses(*selected_verse_range);
                BibleVersesWindow.SetVerses(*selected_verse_range, verses);
                BibleVersesWindow.Open = true;
            }
        }

        BibleVersesWindow.UpdateAndRender(ColorsByWord);

        WordColorsWindow.UpdateAndRender(ColorsByWord);

        if (!CurrentTranslationName.empty())
        {
            std::shared_ptr<BIBLE_DATA::BibleTranslation> current_translation = bible.TranslationsByName.at(CurrentTranslationName);

            std::shared_ptr<BIBLE_DATA::BibleWordIndex> word_index = current_translation->GetWordIndex();
            if (word_index)
            {
                WordStatisticsWindow.UpdateAndRender(word_index->OccurrenceCountsByLowercaseWord);
            }
        }

#if 0
        /// @todo
        if (ImGui::Begin("Indexed Words"))
        {
            using WordAndVerseIds = std::pair<std::string, std::vector<BIBLE_DATA::BibleVerseId>>;
            std::vector<WordAndVerseIds> words_and_bible_verse_ids_by_decreasing_count;
            for (const auto& letter_and_words : bible.BibleVersesByFirstLowercaseLetterThenImportantWord)
            {                
                for (const auto& word_and_bible_verse_ids : letter_and_words.second)
                {
                    words_and_bible_verse_ids_by_decreasing_count.emplace_back(
                        std::make_pair(word_and_bible_verse_ids.first, word_and_bible_verse_ids.second)
                    );
                }
            }

            std::sort(
                words_and_bible_verse_ids_by_decreasing_count.begin(),
                words_and_bible_verse_ids_by_decreasing_count.end(),
                [](const WordAndVerseIds& left, const WordAndVerseIds& right) -> bool
                {
                    return left.second.size() > right.second.size();
                });

            for (const auto& word_and_bible_verse_ids : words_and_bible_verse_ids_by_decreasing_count)
            {
                std::size_t verse_count = word_and_bible_verse_ids.second.size();
                std::string text = word_and_bible_verse_ids.first + " = " + std::to_string(verse_count);
                ImGui::Text(text.c_str());
            }
#if 0
            for (const auto& letter_and_words : bible.BibleVersesByFirstLowercaseLetterThenImportantWord)
            {
                for (const auto& word_and_bible_verse_ids : letter_and_words.second)
                {
                    //bible.TranslationsByName.at("KJV").VersesById[word_and_bible_verse_ids.second];
                    std::size_t verse_count = word_and_bible_verse_ids.second.size();
                    std::string text = word_and_bible_verse_ids.first + " = " + std::to_string(verse_count);
                    ImGui::Text(text.c_str());
                }
            }
#endif
        }
        ImGui::End();
#endif

        MetricsWindow.UpdateAndRender();
        StyleEditorWindow.UpdateAndRender();
        DemoWindow.UpdateAndRender();
        AboutWindow.UpdateAndRender();
    }

    /// Updates the lookup of colors.
    void Gui::UpdateColorLookup(const BIBLE_DATA::Bible& bible)
    {
        // KJV occurrence counts are in comments beside each word.
        static const std::unordered_set<std::string> LOWERCASE_STOP_WORDS =
        {
            "a", // 8177
            "an",
            "and", // 51696
            "be", // 7013
            "for", // 8971
            "he", // 10420
            "him", // 6659
            "his", // 8473
            "i", // 8854
            "in", // 12667
            "is", // 6989
            "it", // 6129
            "not", // 6596
            "of", // 34617
            "that", // 12912
            "the", // 63924
            "them", // 6430
            "they", // 7376
            "to", // 13562
            "was",
            "which", // 4413
            "with", // 6012
        };

        if (CurrentTranslationName.empty())
        {
            return;
        }

        std::shared_ptr<BIBLE_DATA::BibleTranslation> current_translation = bible.TranslationsByName.at(CurrentTranslationName);


        // UPDATE COLORS FOR ANY UNCOLORED WORDS.
        std::random_device random_number_generator;
        ImVec4 color = ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f };
        for (const auto& id_and_verse : current_translation->VersesById)
        {
            const std::vector<BIBLE_DATA::Token>* verse_tokens = id_and_verse.second.GetTokens();
            for (const auto& token : *verse_tokens)
            {
                // SKIP NON-WORD TOKENS.
                bool is_word = (BIBLE_DATA::TokenType::WORD == token.Type);
                if (!is_word)
                {
                    continue;
                }

                std::string lowercase_word = token.Text;
                std::transform(
                    lowercase_word.begin(),
                    lowercase_word.end(),
                    lowercase_word.begin(),
                    [](const char character) { return static_cast<char>(std::tolower(character)); });

                // SKIP STORING COLORS FOR OVERLY COMMON WORDS.
#if __EMSCRIPTEN__
                bool current_word_count_in_stop_words = LOWERCASE_STOP_WORDS.count(lowercase_word);
                bool is_stop_word = (current_word_count_in_stop_words > 0);
#else
                bool is_stop_word = LOWERCASE_STOP_WORDS.contains(lowercase_word);
#endif
                if (is_stop_word)
                {
                    continue;
                }

                // UPDATE THE CURRENT WORDS COLOR.
                /// @todo   How to preserve colors while not re-using colors?
                /// @todo   Upper/lowercase?
                auto current_word_color = ColorsByWord.find(lowercase_word);
                bool current_word_color_exists = (ColorsByWord.cend() != current_word_color);
                if (!current_word_color_exists)
                {
                    // ASSIGN SOME RANDOM COLORS.
                    color.x = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());
                    color.y = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());
                    color.z = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());

                    ColorsByWord[lowercase_word] = color;
                }
            }
        }
    }
}
