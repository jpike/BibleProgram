#include "BibleData/BibleTranslation.h"

namespace BIBLE_DATA
{
    /// Populates a Bible translation from the provided verses.
    /// @param[in]  verses - The verses included in the translation.
    BibleTranslation BibleTranslation::Populate(const std::vector<BibleVerse>& verses, const std::map<BibleBookId, BibleBook>& books)
    {
        BibleTranslation translation;

#if 1
        // ADD ALL VERSES TO THE TRANSLATION.
        for (const auto& verse : verses)
        {
            translation.VersesById[verse.Id] = verse;
        }
#endif
        translation.BooksById = books;

        return translation;
    }

    std::vector<BibleVerse*> BibleTranslation::GetVerses(
        const BibleBookId book,
        const unsigned int chapter_number,
        const unsigned int starting_verse_number,
        const unsigned int ending_verse_number)
    {
        std::vector<BibleVerse*> verses;

        BibleChapter& chapter = BooksById[book].ChaptersByNumber[chapter_number];

        auto chapter_starting_verse = chapter.VersesByNumber.lower_bound(starting_verse_number);
        auto chapter_after_ending_verse = chapter.VersesByNumber.upper_bound(ending_verse_number);

        for (auto id_and_verse = chapter_starting_verse; id_and_verse != chapter_after_ending_verse; ++id_and_verse)
        {
            verses.push_back(&id_and_verse->second);
        }

        return verses;
    }
}
