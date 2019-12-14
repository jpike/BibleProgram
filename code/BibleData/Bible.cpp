#include <unordered_map>
#include "BibleData/Bible.h"

namespace BIBLE_DATA
{
    /// Adds data from the specified translation to the Bible.
    /// @param[in]  translation_name - The name of the translation.
    /// @param[in]  verses - The verses for the translation.
    void Bible::AddTranslation(const std::string& translation_name, const std::vector<BibleVerse>& verses)
    {
        // MAKE SURE THE STRUCTURAL INFORMATION ABOUT THE TRANSLATION IS IN THE BIBLE.
        for (const auto& verse : verses)
        {
            // MAKE SURE THE BOOK EXISTS FOR THE VERSE.
            bool book_already_exists = BooksById.contains(verse.Id.Book);
            if (!book_already_exists)
            {
                BooksById[verse.Id.Book] = BibleBook{ .Id = verse.Id.Book };
            }

            // MAKE SURE THE BOOK CONTAINS THE CHAPTER FOR THE VERSE.
            BibleBook& book = BooksById[verse.Id.Book];
            bool book_contains_chapter = (verse.Id.ChapterNumber <= book.Chapters.size());
            if (!book_contains_chapter)
            {
                book.Chapters.push_back(BibleChapter{ .Book = verse.Id.Book, .Number = verse.Id.ChapterNumber });
            }

            // MAKE SURE THE CHAPTER CONTAINS THE VERSE.
            /// @todo   Revisit this index stuff...Might be better to just have a map to avoid off-by-one errors.
            unsigned int chapter_index = verse.Id.ChapterNumber - 1;
            BibleChapter& chapter = book.Chapters[chapter_index];
            bool chapter_contains_verse = (verse.Id.VerseNumber <= chapter.VerseCount);
            if (!chapter_contains_verse)
            {
                ++chapter.VerseCount;
            }
        }

        // ADD THE TRANSLATION ITSELF.
        BibleTranslation translation = BibleTranslation::Populate(verses);
        TranslationsByName[translation_name] = translation;
    }
}
