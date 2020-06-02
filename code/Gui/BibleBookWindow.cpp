#include <ThirdParty/imgui/imgui.h>
#include "Gui/BibleBookWindow.h"

namespace GUI
{
    /// Updates and renders a single frame of the window, if it's open.
    /// @param[in]  bible - The Bible to display in the window.
    /// @return The selected Bible chapter, if one was selected; null otherwise.
    ///     Pointer is to the chapter as stored within the provided Bible.
    const BIBLE_DATA::BibleChapter* BibleBookWindow::UpdateAndRender(const BIBLE_DATA::Bible& bible)
    {
        // DON'T RENDER ANYTHING IF THE WINDOW ISN'T OPEN.
        if (!Open)
        {
            return nullptr;
        }

        // RENDER THE WINDOW.
        const BIBLE_DATA::BibleChapter* selected_chapter = nullptr;
        if (ImGui::Begin("Books & Chapters", &Open))
        {
            // RENDER EACH BOOK.
            for (const auto& id_and_book : bible.BooksById)
            {
                // RENDER THE CURRENT BOOK WITH A COLLAPSING TREE NODE.
                std::string book_name = BIBLE_DATA::BibleBook::FullName(id_and_book.first);
                if (ImGui::TreeNodeEx(book_name.c_str(), ImGuiTreeNodeFlags_CollapsingHeader))
                {
                    // RENDER A SELECTABLE ITEM FOR EACH CHAPTER.
                    const BIBLE_DATA::BibleBook& book = id_and_book.second;
                    for (const auto& number_and_chapter : book.ChaptersByNumber)
                    {
                        // RENDER A SELECTABLE FOR THE CHAPTER.
                        std::string chapter_text = "Chapter " + std::to_string(number_and_chapter.first);
                        if (ImGui::Selectable(chapter_text.c_str()))
                        {
                            // TRACK THE SELECTED CHAPTER.
                            selected_chapter = &number_and_chapter.second;
                        }
                    }
                }
            }
        }
        ImGui::End();

        // RETURN ANY SELECTED CHAPTER.
        return selected_chapter;
    }
}
