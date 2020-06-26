#include <ThirdParty/imgui/imgui.h>
#include "Gui/BibleBookWindow.h"

namespace GUI
{
    /// Updates and renders a single frame of the window, if it's open.
    /// @return The verse range for any selected chapter; null if no chapter was selected.
    std::optional<BIBLE_DATA::BibleVerseRange> BibleBookWindow::UpdateAndRender()
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return std::nullopt;
        }

        // RENDER THE WINDOW AND GET ANY SELECTED CHAPTER.
        std::optional<BIBLE_DATA::BibleVerseRange> selected_verse_range;
        // Window positioning/sizing is only done upon the first use to allow preserving a user's manual changes.
        // The window is positioned to basically be in the top-left corner of the window after the main menu bar.
        ImVec2 current_drawing_cursor_position = ImGui::GetCursorPos();
        ImGui::SetNextWindowPos(current_drawing_cursor_position, ImGuiCond_FirstUseEver);
        // The width is set to be wide enough for all book titles to be displayed.
        constexpr float MAX_BOOK_TITLE_WIDTH_IN_PIXELS = 200.0f;
        ImGuiIO& io = ImGui::GetIO();
        float available_vertical_screen_space_in_pixels = io.DisplaySize.y - current_drawing_cursor_position.y;
        ImVec2 window_size_in_pixels(MAX_BOOK_TITLE_WIDTH_IN_PIXELS, available_vertical_screen_space_in_pixels);
        ImGui::SetNextWindowSize(window_size_in_pixels, ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Books & Chapters", &Open))
        {
            // RENDER EACH BOOK.
            for (const auto& id_and_book : BIBLE_DATA::Bible::BOOKS_BY_ID)
            {
                // RENDER THE CURRENT BOOK WITH A COLLAPSING TREE NODE.
                std::string book_name = BIBLE_DATA::BibleBook::FullName(id_and_book.first);
                if (ImGui::TreeNodeEx(book_name.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
                {
                    // RENDER A SELECTABLE ITEM FOR EACH CHAPTER.
                    const BIBLE_DATA::BibleBook& book = id_and_book.second;
                    std::size_t chapter_count = book.VerseCountsByChapter.size();
                    for (std::size_t chapter_index = 0; chapter_index < chapter_count; ++chapter_index)
                    {
                        // RENDER A SELECTABLE FOR THE CHAPTER.
                        unsigned int chapter_number = static_cast<unsigned int>(chapter_index + 1);
                        std::string chapter_text = "Chapter " + std::to_string(chapter_number);
                        std::string chapter_id = "##" + book_name + chapter_text;
                        std::string chapter_text_and_id = chapter_text + chapter_id;
                        if (ImGui::Selectable(chapter_text_and_id.c_str()))
                        {
                            // TRACK THE SELECTED CHAPTER.
                            unsigned int chapter_verse_count = book.VerseCountsByChapter[chapter_index];
                            selected_verse_range = BIBLE_DATA::BibleVerseRange
                            {
                                .StartingVerse = BIBLE_DATA::BibleVerseId
                                {
                                    .Book = id_and_book.first,
                                    .ChapterNumber = chapter_number,
                                    // All chapters begin at verse 1.
                                    .VerseNumber = 1
                                },
                                .EndingVerse = BIBLE_DATA::BibleVerseId
                                {
                                    .Book = id_and_book.first,
                                    .ChapterNumber = chapter_number,
                                    .VerseNumber = chapter_verse_count
                                },
                            };
                        }
                    }
                }
            }
        }
        ImGui::End();

        // RETURN ANY SELECTED CHAPTER.
        return selected_verse_range;
    }
}
