#include "BibleData/BibleBook.h"

namespace BIBLE_DATA
{
    /// Adds a verse to the appropriate chapter of the book.
    /// @param[in]  verse - The verse to add.
    void BibleBook::Add(const BibleVerse& verse)
    {
        /// @todo   Make this more robust.

        // MAKE SURE THE APPROPRIATE CHAPTER EXISTS.
        std::size_t current_chapter_index = verse.ChapterNumber - 1;
        bool chapter_exists = (current_chapter_index < Chapters.size());
        if (!chapter_exists)
        {
            Chapters.push_back(BibleChapter{ .Number = verse.ChapterNumber});
        }

        // ADD THE VERSE.
        Chapters[current_chapter_index].Verses.push_back(verse);
    }
}
