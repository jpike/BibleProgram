#include <algorithm>
#include <cctype>
#include <random>
#include <unordered_set>
#include <ThirdParty/imgui/imgui_internal.h>
#include "BibleData/BibleBook.h"
#include "Gui/BibleVersesWindow.h"

namespace GUI
{
    /// Updates and renders a single frame of the window, if it's open.
    void BibleVersesWindow::UpdateAndRender()
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return;
        }

        // RENDER EACH VERSE IN THE WINDOW.
        std::string window_title = (
            BIBLE_DATA::BibleBook::FullName(StartingVerseId.Book) + " " +
            std::to_string(StartingVerseId.ChapterNumber) + ":" +
            std::to_string(StartingVerseId.VerseNumber));
        bool multiple_verses = Verses.size() > 1;
        if (multiple_verses)
        {
            window_title += " - " +
                BIBLE_DATA::BibleBook::FullName(EndingVerseId.Book) + " " +
                std::to_string(EndingVerseId.ChapterNumber) + ":" +
                std::to_string(EndingVerseId.VerseNumber);
        }
        std::string window_title_and_id = window_title + "###Verses";

        ImVec2 min_window_size_in_pixels = { 400.0f, 400.0f };
        ImGui::SetNextWindowSize(min_window_size_in_pixels, ImGuiCond_FirstUseEver);

        static std::vector<std::pair<unsigned int, std::string>> counts_with_words;

        if (ImGui::Begin(window_title_and_id.c_str(), &Open, ImGuiWindowFlags_MenuBar))
        {
            // RENDER A MENU BAR.
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::Button("Statistics"))
                {
                    ComputeWordStatistics();
                    counts_with_words.clear();
                    for (const auto& word_with_count : OccurrenceCountsByWord)
                    {
                        counts_with_words.emplace_back(word_with_count.second, word_with_count.first);
                    }
                    std::sort(counts_with_words.begin(), counts_with_words.end());
                    std::reverse(counts_with_words.begin(), counts_with_words.end());
                }

                if (ImGui::Button("Colorize"))
                {
                    UpdateColorLookup();
                }

                ImGui::EndMenuBar();
            }

            // RENDER THE VERSES.
            BIBLE_DATA::BibleBookId previous_book = BIBLE_DATA::BibleBookId::INVALID;
            unsigned int previous_chapter_number = 0;
            for (const auto& verse : Verses)
            {
                // PRINT SOME TEXT TO INDICATE IF A NEW BOOK IS BEING STARTED.
                bool new_book_being_started = (previous_book != verse.Id.Book);
                if (new_book_being_started)
                {
                    std::string book_name = BIBLE_DATA::BibleBook::FullName(verse.Id.Book);
                    std::transform(
                        book_name.begin(), 
                        book_name.end(), 
                        book_name.begin(), 
                        [](const char character) { return static_cast<char>(std::toupper(character)); });
                    ImGui::TextDisabled(book_name.c_str());

                    previous_book = verse.Id.Book;
                }
                // PRINT SOME TEXT TO INDICATE IF A NEW CHAPTER IS BEING STARTED.
                bool new_chapter_being_started = (previous_chapter_number != verse.Id.ChapterNumber);
                if (new_chapter_being_started)
                {
                    std::string chapter_text = "CHAPTER " + std::to_string(verse.Id.ChapterNumber);
                    ImGui::TextDisabled(chapter_text.c_str());

                    previous_chapter_number = verse.Id.ChapterNumber;
                }

                bool header_info_printed = (new_book_being_started || new_chapter_being_started);
                if (header_info_printed)
                {
                    ImGui::NewLine();
                }

                Render(verse);
            }

            bool any_verse_hovered = ImGui::IsAnyItemHovered();
            if (!any_verse_hovered)
            {
                CurrentlyHighlightedWord.clear();
            }
        }
        ImGui::End();

        /// @todo   Make this a separate window!
        if (ImGui::Begin("Word Counts"))
        {
            for (const auto& count_with_word : counts_with_words)
            {
                std::string word_statistics = count_with_word.second + ": " + std::to_string(count_with_word.first);
                ImGui::Text(word_statistics.c_str());
            }
        }
        ImGui::End();
    }

    /// Renders a single Bible verse in the window.
    /// @param[in]  verse - The verse to render.
    void BibleVersesWindow::Render(const BIBLE_DATA::BibleVerse& verse)
    {
        //ImVec4 dark_gray_verse_number_color = { 0.5f, 0.5f, 0.5f, 1.0f };
        std::string verse_number = std::to_string(verse.Id.VerseNumber);
        ImGui::SameLine();
        ImGui::TextDisabled(verse_number.c_str());

        // MAIN LOGIC.
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        ImVec4 color;
        color.x = 1.0f;
        color.y = 1.0f;
        color.z = 1.0f;
        color.w = 1.0f;
        const std::vector<BIBLE_DATA::Token>* verse_tokens = verse.GetTokens();
        for (std::size_t token_index = 0; token_index < verse_tokens->size(); ++token_index)
        {
            const auto& token = verse_tokens->at(token_index);
            if (token.Type == BIBLE_DATA::TokenType::SPACE)
            {
                continue;
            }

            // GET THE CURRENT WORD'S COLOR.
            auto current_word_color = ColorsByWord.find(token.Text);
            bool current_word_color_exists = (ColorsByWord.cend() != current_word_color);
            if (current_word_color_exists)
            {
                color = current_word_color->second;
            }
            else
            {
                // White.
                color = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
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
            bool is_currently_highlighted_word = (token.Text == CurrentlyHighlightedWord) || (ImGui::IsItemHovered());
            if (is_currently_highlighted_word)
            {
                /// @todo   Something other than yellow for highlights?
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));

                CurrentlyHighlightedWord = token.Text;
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
#if RANDOM_COLORS
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
#endif
        }
    }

    /// Updates the lookup of colors for the current set of verses.
    void BibleVersesWindow::UpdateColorLookup()
    {
        const std::unordered_set<std::string> STOP_WORDS =
        {
            "a",
            "the"
        };

        // UPDATE COLORS FOR ANY UNCOLORED WORDS.
        std::random_device random_number_generator;
        ImVec4 color = ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f };
        for (const BIBLE_DATA::BibleVerse& verse : Verses)
        {
            const std::vector<BIBLE_DATA::Token>* verse_tokens = verse.GetTokens();
            for (const auto& token : *verse_tokens)
            {
                // SKIP NON-WORD TOKENS.
                bool is_word = (BIBLE_DATA::TokenType::WORD == token.Type);
                if (!is_word)
                {
                    continue;
                }

                // SKIP STORING COLORS FOR OVERLY COMMON WORDS.
                bool is_stop_word = STOP_WORDS.contains(token.Text);
                if (is_stop_word)
                {
                    continue;
                }

                // UPDATE THE CURRENT WORDS COLOR.
                /// @todo   How to preserve colors while not re-using colors?
                /// @todo   Upper/lowercase?
                auto current_word_color = ColorsByWord.find(token.Text);
                bool current_word_color_exists = (ColorsByWord.cend() != current_word_color);
                if (!current_word_color_exists)
                {
                    // ASSIGN SOME RANDOM COLORS.
                    color.x = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());
                    color.y = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());
                    color.z = static_cast<float>(random_number_generator()) / static_cast<float>(random_number_generator.max());

                    ColorsByWord[token.Text] = color;
                }
            }
        }
    }

    /// Computes statistics about the words currently displayed in the window.
    void BibleVersesWindow::ComputeWordStatistics()
    {
        OccurrenceCountsByWord.clear();

        for (const BIBLE_DATA::BibleVerse& verse : Verses)
        {
            const std::vector<BIBLE_DATA::Token>* verse_tokens = verse.GetTokens();
            for (const auto& token : *verse_tokens)
            {
                // SKIP NON-WORD TOKENS.
                bool is_word = (BIBLE_DATA::TokenType::WORD == token.Type);
                if (!is_word)
                {
                    continue;
                }

                // COUNT THE WORD.
                ++OccurrenceCountsByWord[token.Text];
            }
        }
    }
}
